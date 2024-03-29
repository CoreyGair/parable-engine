#include "Renderer.h"

#include "pblpch.h"
#include "Core/Base.h"

// stuff for rotation code when updating mvp matrices
// temp
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
//

// obj file loader, again temp
//#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <vulkan/vulkan.hpp>
#include "Wrapper/VulkanWrapper.h"
#include "Wrapper/VulkanExceptions.h"

#include "Loader/Loader.h"

#include "Mesh/MeshStore.h"
#include "Mesh/Mesh.h"

#include "Texture/TextureStore.h"
#include "Texture/Texture.h"

#include "Asset/Handle.h"

#include <GLFW/glfw3.h>

#include "Window/Window.h"

#include <fstream>

#include "UniformBufferObjects.h"


namespace Parable::Vulkan
{



Renderer::Renderer(GLFWwindow* window) : m_window(window)
{
    // CREATE instance
    vk::ApplicationInfo applicationInfo(
        // TODO: set this to the game name somehow
        "APP_NAME_HERE",            // app name
        VK_MAKE_VERSION(1, 0, 0),   // app ver
        "Parable Engine",           // engine name
        // TODO: set to parable version
        VK_MAKE_VERSION(1, 0, 0),   // engine ver
        VK_API_VERSION_1_3         // vulkan ver
    );

    std::vector<const char*> validationLayers;
#ifdef PBL_DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    
    std::vector<const char*> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // find required glfw extensions
    uint32_t glfwExtensionCount = 0;
    const char** pGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> glfwExtensions(pGlfwExtensions, pGlfwExtensions+glfwExtensionCount);

    //// pretty sure this is wrong/dont do this as glfwGetRequiredInstanceExtensions is for instance and requiredDeviceExtensions is for device ////
    //
    // // add the glfw extensions to the other required extensions
    // requiredDeviceExtensions.reserve(requiredDeviceExtensions.size() + glfwExtensionCount);
    // for (size_t i = 0; i < glfwExtensionCount; ++i)
    // {
    //     const char* glfwExt = glfwExtensions[i];
    //     if (find_if(
    //             requiredDeviceExtensions.cbegin(),
    //             requiredDeviceExtensions.cend(),
    //             [&glfwExt](const char* ext) { return strcmp(ext,glfwExt) == 0; }
    //         ) 
    //         == requiredDeviceExtensions.cend())
    //     {
    //         requiredDeviceExtensions.push_back(glfwExt);
    //     }
    // }

    vk::InstanceCreateInfo instanceCreateInfo(
        {}, 
        &applicationInfo,            // app info
        validationLayers,           // enabled layers
        glfwExtensions    // enabled extensions
    );

    m_instance = vk::createInstance(instanceCreateInfo);

    // CREATE surface
    {
        VkSurfaceKHR renderSurface;
        glfwCreateWindowSurface(m_instance, window, nullptr, &renderSurface);
        m_surface = vk::SurfaceKHR(renderSurface);
    }

    // CREATE physical device
    {
        PhysicalDevicePicker physicalDevicePicker;

        // needs geom shaders & anisotropy
        // TODO: optionally use anisotropy based on avail
        physicalDevicePicker.add_filter([](vk::PhysicalDevice& pd) {
            auto features = pd.getFeatures();
            return features.geometryShader && features.samplerAnisotropy;
        });

        // need all queues
        physicalDevicePicker.add_filter([&](vk::PhysicalDevice& pd) {
            return PhysicalDevice::get_optional_queue_family_indices_from_physical_device(pd, m_surface).is_complete();
        });

        // need required exts
        physicalDevicePicker.add_filter([&](vk::PhysicalDevice& pd) {
            std::vector<vk::ExtensionProperties> availableExtensions = pd.enumerateDeviceExtensionProperties();

            std::set<std::string> requiredExtensionsSet(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

            for (const auto& extension : availableExtensions) {
                requiredExtensionsSet.erase(extension.extensionName);
            }

            return requiredExtensionsSet.empty();
        });

        // need swapchain support for the surface
        physicalDevicePicker.add_filter([&](vk::PhysicalDevice& pd) {
            SwapChainSupportDetails support = PhysicalDevice::get_swapchain_support_details_from_physical_device(pd, m_surface);
            return !support.formats.empty() && !support.present_modes.empty();
        });

        // prioritise discrete
        physicalDevicePicker.add_scorer([&](vk::PhysicalDevice& pd) {
            return pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? 10000 : 0;
        });

        // prioritise high mem
        physicalDevicePicker.add_scorer([&](vk::PhysicalDevice& pd) {
            auto heaps = pd.getMemoryProperties().memoryHeaps;
            vk::DeviceSize max_mem_size = 0;
            for (const auto& heap : heaps)
            {
                if (heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                {
                    // Device local heap, should be size of total GPU VRAM.
                    // heap.size will be the size of VRAM in bytes. (bigger is better)
                    if (heap.size > max_mem_size) max_mem_size = heap.size;
                }
            }

            // add size in MB to score
            return (int)max_mem_size / 1000000;
        });

        m_physical_device = PhysicalDevice(m_instance, physicalDevicePicker);
    }

    // get queue families
    QueueFamilyIndices queueFamilyIndices = m_physical_device.get_queue_family_indices(m_surface);

    std::set<uint32_t> uniqueQueueFamilies;
    uniqueQueueFamilies.insert(queueFamilyIndices.graphics_family);
    uniqueQueueFamilies.insert(queueFamilyIndices.presentation_family);
    uniqueQueueFamilies.insert(queueFamilyIndices.transfer_family);
    
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(uniqueQueueFamilies.size());

    // setup the queue creation data
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo(
            {},
            queueFamily,        // family index
            1,                  // queue count
            &queuePriority     // priorities
        );
        
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // specify the device features we need
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = true;

    // creating the actual logical device
    vk::DeviceCreateInfo deviceCreateInfo(
        {},
        queueCreateInfos,
        validationLayers,
        requiredDeviceExtensions,
        &deviceFeatures
    );
    
    m_device = Device(m_physical_device, deviceCreateInfo);

    // GET QUEUES
    m_graphics_queue = (*m_device).getQueue(queueFamilyIndices.graphics_family, 0);
    m_present_queue = (*m_device).getQueue(queueFamilyIndices.presentation_family, 0);
    m_transfer_queue = (*m_device).getQueue(queueFamilyIndices.transfer_family, 0);

    // CREATE SWAP CHAIN
    m_swapchain = Swapchain(m_device, m_physical_device, m_surface, window);

    PBL_CORE_ASSERT(m_swapchain.get_views().size() > 0);

    // CREATE RENDERPASS

    RenderpassBuilder renderpassBuilder;

    uint32_t colorAttachmentIndex = renderpassBuilder.add_attachment(vk::AttachmentDescription(
        {},
        m_swapchain.get_image_format(),
        vk::SampleCountFlagBits::e1,        // samples
        vk::AttachmentLoadOp::eClear,       // load op, how are contents treated at start of pass (here, we want to clear it)
        vk::AttachmentStoreOp::eStore,      // store op, how are contents treated at end of pass (here, we store to mem)
        vk::AttachmentLoadOp::eDontCare,    // stencil load op
        vk::AttachmentStoreOp::eDontCare,   // stencil store op
        vk::ImageLayout::eUndefined,        // initial layout
        vk::ImageLayout::ePresentSrcKHR     // final layout
    ));
    uint32_t colorAttachmentRefIndex = renderpassBuilder.add_attachment_ref(colorAttachmentIndex, vk::ImageLayout::eColorAttachmentOptimal);

    uint32_t depthAttachmentIndex = renderpassBuilder.add_attachment(vk::AttachmentDescription(
        {},
        vk::Format::eD32Sfloat, // TODO: must be same as m_depth_image format
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    ), AttachmentFormat::CONSTANT);
    uint32_t depthAttachmentRefIndex = renderpassBuilder.add_attachment_ref(depthAttachmentIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    uint32_t colorSubpassIndex = renderpassBuilder.add_subpass(vk::SubpassDescription(
        {},
        vk::PipelineBindPoint::eGraphics,
        0,          // inputAttachmentCount - want to ignore these as we are refing a color attachment instead
        nullptr,    // pInputAttachments
        1,
        &renderpassBuilder.get_attachment_refs()[colorAttachmentRefIndex],
        nullptr, // pResolveAttachments
        &renderpassBuilder.get_attachment_refs()[depthAttachmentRefIndex]
    ));

    renderpassBuilder.add_subpass_dependency(vk::SubpassDependency(
        VK_SUBPASS_EXTERNAL,    // src subpass, refer to the implicit subpasses before and after our actual one
        colorSubpassIndex,      // dest subpass, refers to our 1 and only subpass
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        {}, // src access mask
        vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite // dst access mask
    ));

    m_renderpass = renderpassBuilder.create(m_device, m_swapchain);

    // CREATE layout for the per-frame descriptor set

    vk::DescriptorSetLayoutBinding perFrameDescriptorSetBindings[] = {
        // bindig for view + projection matrices
        vk::DescriptorSetLayoutBinding(
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex,
            {}
        )
    };
    
    m_frame_descriptor_set_layout = (*m_device).createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(
        {},
        perFrameDescriptorSetBindings
    ));

    // CREATE layout for the per-material

    vk::DescriptorSetLayoutBinding perMaterialDescriptorSetBindings[] = {
        // bindig for texture
        vk::DescriptorSetLayoutBinding(
            0,
            vk::DescriptorType::eCombinedImageSampler,
            1,
            vk::ShaderStageFlagBits::eFragment,
            {}
        )
    };
    
    m_material_descriptor_set_layout = (*m_device).createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(
        {},
        perMaterialDescriptorSetBindings
    ));


    // CREATE layout for the per-draw descriptor set

    // vk::DescriptorSetLayoutBinding perDrawDescriptorSetBindings[] = {
    //     // binding for model mat
    //     // vk::DescriptorSetLayoutBinding(
    //     //     0,
    //     //     vk::DescriptorType::eUniformBuffer,
    //     //     1,
    //     //     vk::ShaderStageFlagBits::eVertex,
    //     //     {}
    //     // ),
    //     // bindig for combined tex sampler
    //     // would become part of per material set
    //     // TODO: replace with binding for tex + sampler (more efficient?)
    //     // vk::DescriptorSetLayoutBinding(
    //     //     1,
    //     //     vk::DescriptorType::eCombinedImageSampler,
    //     //     1,
    //     //     vk::ShaderStageFlagBits::eFragment,
    //     //     {}
    //     // )
    // };
    
    // m_draw_descriptor_set_layout = (*m_device).createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(
    //     {},
    //     perDrawDescriptorSetBindings
    // ));

    // CREATE PIPELINE

    vk::DescriptorSetLayout descSetLayouts[] = {
        m_frame_descriptor_set_layout,
        m_material_descriptor_set_layout,
        // m_draw_descriptor_set_layout
    };

    // using push constants for model matrix
    vk::PushConstantRange pushConstRanges[] = {
        vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4))
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
        {},
        descSetLayouts,
        pushConstRanges
    );

    m_pipeline_layout = (*m_device).createPipelineLayout(pipelineLayoutInfo);

    // load shaders
    vk::ShaderModule vertShaderModule = load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Renderer\\Shaders\\vert.spv");
    vk::ShaderModule fragShaderModule = load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Renderer\\Shaders\\frag.spv");

    // push these to be destroyed only when we will never recreate the pipeline again
    // (atm when the Renderer is destroyed)
    m_shader_modules.push_back(vertShaderModule);
    m_shader_modules.push_back(fragShaderModule);

    GraphicsPipelineBuilder graphicsPipelineBuilder;
    
    graphicsPipelineBuilder.info.inputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        VK_FALSE
    );

    graphicsPipelineBuilder.info.viewport.width = (float)m_swapchain.get_extent().width;
    graphicsPipelineBuilder.info.viewport.height = (float)m_swapchain.get_extent().height;
    graphicsPipelineBuilder.info.viewport.minDepth = 0.0f;
    graphicsPipelineBuilder.info.viewport.maxDepth = 1.0f;

    graphicsPipelineBuilder.info.scissor.extent = m_swapchain.get_extent();

    graphicsPipelineBuilder.info.rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo(
        {},
        VK_FALSE, // depthClampEnable,
        VK_FALSE, // rasterizerDiscardEnable
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eCounterClockwise,
        VK_FALSE, // depthBiasEnable
        {}, // depthBiasConstFactor
        {}, // depthBiasClamp
        {}, // depthBiasSlopeFactor
        1.0f //lineWdith
    );

    graphicsPipelineBuilder.info.depthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo(
        {},
        true, // depthTestEnable
        true, // depthWriteEnable
        vk::CompareOp::eLess, // depthCompareOp
        false, // depthBoundsTestEnable
        false  // stencilTestEnable
    );

    graphicsPipelineBuilder.info.multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo(
        {},
        vk::SampleCountFlagBits::e1,
        VK_FALSE // sampleShadingEnabled
    );

    graphicsPipelineBuilder.add_shader_stage(vk::PipelineShaderStageCreateInfo(
        {},
        vk::ShaderStageFlagBits::eVertex,
        vertShaderModule,
        "main"
    ));
    graphicsPipelineBuilder.add_shader_stage(vk::PipelineShaderStageCreateInfo(
        {},
        vk::ShaderStageFlagBits::eFragment,
        fragShaderModule,
        "main"
    ));

    // basic alpha blending
    graphicsPipelineBuilder.add_blend_attachment(vk::PipelineColorBlendAttachmentState(
        VK_TRUE,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    ));

    graphicsPipelineBuilder.add_binding_description(Vertex::getBindingDescription());

    auto attachment_descs = Vertex::getAttributeDescriptions();
    for (auto a : attachment_descs)
    {
        graphicsPipelineBuilder.add_attachment_description(std::move(a));
    }

    m_graphics_pipeline = graphicsPipelineBuilder.create(m_device, m_pipeline_layout, m_renderpass, colorSubpassIndex);

    // CREATE COMMAND POOL
    vk::CommandPoolCreateInfo cmdPoolInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueFamilyIndices.graphics_family
    );

    m_command_pool = CommandPool(m_device, cmdPoolInfo);

    // CREATE depth image
    vk::ImageCreateInfo depthImgCreateInfo = vk::ImageCreateInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eD32Sfloat, // TODO: in future, check if this is avaliable or use suitable alternative
        // ALSO TODO: investigate performance between this 32bit float and a 24bit format (aparently will be faster to use a 24bit integer format, unless high precision really needed)
        {m_swapchain.get_extent().width, m_swapchain.get_extent().height, 1},
        1, // mipLevels
        1, // arrayLayers
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::SharingMode::eExclusive,
        0, nullptr, // queueFamilyIndices for concurrent sharing
        vk::ImageLayout::eUndefined
    );
    m_depth_image = RecreatableImage(m_device, m_physical_device, depthImgCreateInfo);

    m_depth_image_view = (*m_device).createImageView(vk::ImageViewCreateInfo(
        {},
        m_depth_image,
        vk::ImageViewType::e2D,
        vk::Format::eD32Sfloat, // TODO: must be same as m_depth_image_format
        {},
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eDepth,
            0, // baseMipLevel
            1, // levelCount
            0, // baseArrayLayer
            1  // layerCount
        )
    ));

    // CREATE FRAMEBUFFERS
    // dependent on m_depth_image_view, so after its creation above
    m_framebuffers = Framebuffers(m_device, m_swapchain.get_views(), {m_depth_image_view}, m_renderpass, m_swapchain.get_extent());

    // CREATE per frame uniform buffers
    vk::DeviceSize perFrameUniformBufferSize = sizeof(PerFrameUniformBufferObject);

    BufferBuilder perFrameUniformBufferBuilder;
    perFrameUniformBufferBuilder.buffer_info.size = perFrameUniformBufferSize;
    perFrameUniformBufferBuilder.buffer_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    perFrameUniformBufferBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    perFrameUniformBufferBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        perFrameUniformBufferBuilder.emplace_back_vector(m_frame_uniform_buffers, m_device, m_physical_device);

        // keep persistently mapped
        m_frame_uniform_buffers[i].map(0, perFrameUniformBufferSize);
    }
    
    // CREATE pool for frame descriptor sets
    vk::DescriptorPoolSize frameDescriptorPoolSizes[] = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        )
    };

    m_frame_descriptor_pool = (*m_device).createDescriptorPool(vk::DescriptorPoolCreateInfo(
        {},
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
        frameDescriptorPoolSizes
    ));

    // now done in tex store
    // // CREATE pool for material descriptor sets
    // // for now, allocates just a large amt for dev
    // vk::DescriptorPoolSize materialDescriptorPoolSizes[] = {
    //     vk::DescriptorPoolSize(
    //         vk::DescriptorType::eCombinedImageSampler,
    //         static_cast<uint32_t>(1000)
    //     )
    // };

    // m_material_descriptor_pool = (*m_device).createDescriptorPool(vk::DescriptorPoolCreateInfo(
    //     {},
    //     static_cast<uint32_t>(1000),
    //     materialDescriptorPoolSizes
    // ));

    // CREATE frame descriptor sets
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_frame_descriptor_set_layout);
    m_frame_descriptor_sets = DescriptorSets(m_device, m_frame_descriptor_pool, layouts);

    // populate descriptors
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vk::DescriptorBufferInfo uniformBufferInfo(
            m_frame_uniform_buffers[i],
            0,
            sizeof(PerFrameUniformBufferObject)
        );

        vk::WriteDescriptorSet descriptorWrites[] = {
            vk::WriteDescriptorSet(
                m_frame_descriptor_sets[i],
                0, // binding
                0, // arrayElement
                1, // descriptorCount
                vk::DescriptorType::eUniformBuffer,
                {},                     // pImageInfo
                &uniformBufferInfo,     // pBufferInfo
                {}                      // pTexelBufferView
            )
        };

        (*m_device).updateDescriptorSets(descriptorWrites, {});
    }

    // Write static view and proj mats
    // temp for now
    // TODO: the projection matrix needs to change with screen resolution (aspect ratio change)
    //  or just have some Renderer.set_resolution which rebuilds necessary stuff
    //      (i think this is better, as lends itself to having a game setting for resolution and keeping it fixed at that)
    {
        PerFrameUniformBufferObject ubo{};
        
        ubo.view = glm::lookAt(glm::vec3(0.0f, 3.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // perspective projection, 45 degree vertical fov
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain.get_extent().width / (float) m_swapchain.get_extent().height, 0.1f, 10.0f);

        // glm designed for opengl
        // opengl Y coord is inverted
        ubo.proj[1][1] *= -1;
        
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            memcpy(m_frame_uniform_buffers[i].get_map(), &ubo, sizeof(ubo));
        }
    }

    // CREATE COMMAND BUFFERS
    m_command_buffers = m_command_pool.create_command_buffers(vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);

    // CREATE resource loader

    m_resource_loader = std::make_unique<Loader>(m_device, m_physical_device, queueFamilyIndices.transfer_family);

    // CREATE mesh store

    // vertex + index bufs
    BufferBuilder vertex_buffer_builder;
    vertex_buffer_builder.buffer_info.size = MB(2);
    vertex_buffer_builder.buffer_info.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vertex_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertex_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    Buffer vertex_buffer = vertex_buffer_builder.create(m_device, m_physical_device);

    BufferBuilder index_buffer_builder;
    index_buffer_builder.buffer_info.size = MB(2);
    index_buffer_builder.buffer_info.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    index_buffer_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    index_buffer_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    Buffer index_buffer = index_buffer_builder.create(m_device, m_physical_device);

    m_mesh_store = std::make_unique<MeshStore>(vertex_buffer, index_buffer, *m_resource_loader);

    // CREATE tex store
    m_texture_store = std::make_unique<TextureStore>(m_device, m_material_descriptor_set_layout, *m_resource_loader);
}

Renderer::~Renderer()
{
    // TODO: make sure this is complete
    // ive added a lot to this class, not sure ive kept the cleanup up to date

    // wait until the device has finished working
    (*m_device).waitIdle();

    m_framebuffers.destroy();
    m_swapchain.destroy();

    m_graphics_pipeline.destroy();
    (*m_device).destroyPipelineLayout(m_pipeline_layout);
    m_renderpass.destroy();

    for (auto& buf : m_frame_uniform_buffers)
    {
        buf.destroy();
    }

    (*m_device).destroyDescriptorPool(m_frame_descriptor_pool);
    (*m_device).destroyDescriptorPool(m_draw_descriptor_pool);

    for (int i = 0; i < m_textures.size(); ++i)
    {
        (*m_device).destroySampler(m_texture_samplers[i]);
        (*m_device).destroyImageView(m_texture_views[i]);
        m_textures[i].destroy();
    }

    (*m_device).destroyDescriptorSetLayout(m_frame_descriptor_set_layout);
    (*m_device).destroyDescriptorSetLayout(m_material_descriptor_set_layout);

    m_command_pool.destroy();

    for (auto m : m_shader_modules)
    {
        (*m_device).destroyShaderModule(m);
    }

    m_device.destroy();

    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}

Handle<Parable::Mesh> Renderer::load_mesh(AssetDescriptor descriptor)
{
    return m_mesh_store->load(descriptor);
}

/**
 * Currently just loads a texture, with the material handle pointing to said texture.
 * 
 * @param texturePath 
 * @return MaterialHandle 
 */
Handle<Parable::Texture> Renderer::load_texture(AssetDescriptor descriptor)
{
    return m_texture_store->load(descriptor);
}

/**
 * Recreates the swapchain and all dependent components.
 * 
 * Useful when a property about the swapchain changes (e.g. window resize)
 */
void Renderer::recreate_swapchain()
{
    PBL_CORE_TRACE("Vulkan: Swapchain recreated.");

    m_swapchain.recreate_swapchain();

    PBL_CORE_TRACE("Swapchain new extent: {} x {}", m_swapchain.get_extent().width, m_swapchain.get_extent().height);

    m_renderpass.recreate_renderpass(m_swapchain);

    m_graphics_pipeline.recreate_pipeline(m_pipeline_layout, m_renderpass);

    (*m_device).destroyImageView(m_depth_image_view);
    m_depth_image.info.extent = vk::Extent3D(m_swapchain.get_extent().width, m_swapchain.get_extent().height, 1);
    m_depth_image.recreate(m_device, m_physical_device);
    m_depth_image_view = (*m_device).createImageView(vk::ImageViewCreateInfo(
        {},
        m_depth_image,
        vk::ImageViewType::e2D,
        vk::Format::eD32Sfloat, // TODO: must be same as m_depth_image_format
        {},
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eDepth,
            0, // baseMipLevel
            1, // levelCount
            0, // baseArrayLayer
            1  // layerCount
        )
    ));

    m_framebuffers.recreate_framebuffers(m_swapchain.get_views(), {m_depth_image_view}, m_renderpass, m_swapchain.get_extent());
}

void Renderer::record_command_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    // tel vulkan we are starting to record to a command buff
    commandBuffer.begin(vk::CommandBufferBeginInfo());

    // start a render pass
    std::vector<vk::ClearValue> clearValues = {
        vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f),
        vk::ClearDepthStencilValue(1.0f, 0)
    };
    vk::RenderPassBeginInfo renderpassInfo(
        m_renderpass,
        m_framebuffers[imageIndex].framebuffer,
        vk::Rect2D({0,0}, m_swapchain.get_extent()), 
        clearValues
    );

    // the render pass begins
    commandBuffer.beginRenderPass(renderpassInfo, vk::SubpassContents::eInline);

    // bind the pipeline
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphics_pipeline);

    // bind per frame descriptor set
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline_layout, 0, 1, &m_frame_descriptor_sets[m_current_frame], 0, nullptr);

    // draw meshes
    DrawCall* prevDrawCall = nullptr;
    for (auto& drawCall : m_draw_calls)
    {   
        if (!drawCall.mesh.is_loaded()) {
            // this mesh is not finished loading, skip call
            PBL_CORE_TRACE("Skipping due to unloaded mesh.");
            continue;
        }

        const Mesh& m = static_cast<const Mesh&>(*drawCall.mesh);

        if (prevDrawCall == nullptr || prevDrawCall->mesh != drawCall.mesh)
        {
            commandBuffer.bindVertexBuffers(0, {m_mesh_store->get_vertex_buffer()}, {m.get_vertex_slice().start});
            commandBuffer.bindIndexBuffer(m_mesh_store->get_index_buffer(), m.get_index_slice().start, vk::IndexType::eUint32);
        }

        if (!drawCall.texture.is_loaded()) {
            // this tex is not finished loading, skip call
            PBL_CORE_TRACE("Skipping due to unloaded texture.");
            continue;
        }

        if (prevDrawCall == nullptr || prevDrawCall->texture != drawCall.texture)
        {
            const Texture& t = static_cast<const Texture&>(*drawCall.texture);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline_layout, 1, 1, &t.get_descriptor_set(), 0, nullptr);
        }

        commandBuffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &drawCall.transform);

        commandBuffer.drawIndexed(m.get_index_count(), 1, 0, 0, 0);

        prevDrawCall = &drawCall;
    }

    // end render pass
    commandBuffer.endRenderPass();

    // stop recording to buff
    commandBuffer.end();

    // clear draw calls, we are done with them this frame
    m_draw_calls.clear();
}

/**
 * Called every frame, draw stuff
 */
void Renderer::on_update()
{
    if (m_paused)
    {
        // dont draw if paused (usually when window minimised)
        return;
    }    

    m_resource_loader->run_tasks();

    const FramebufferData& framebufferData = m_framebuffers[m_current_frame];

    // wait for prev frame to finish
    vk::Result waitResult = (*m_device).waitForFences(1, &framebufferData.inflight_fence, VK_TRUE, UINT64_MAX);
    if (waitResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for inflight fence");
    }

    // aquire image from the swapchain
    vk::ResultValue<uint32_t> imageResult = (*m_device).acquireNextImageKHR(m_swapchain, UINT64_MAX, framebufferData.image_available_sem, VK_NULL_HANDLE);
    
    // recreate the swap chain stuff if needed
    if (imageResult.result == vk::Result::eErrorOutOfDateKHR || imageResult.result == vk::Result::eSuboptimalKHR || m_resized) {
        m_resized = false;
        recreate_swapchain();
        return;
    } else if (imageResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    uint32_t imageIndex = imageResult.value;

    // reset the fence once we know we will render stuff
    vk::Result resetResult = (*m_device).resetFences(1, &framebufferData.inflight_fence);
    if (resetResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to reset inflight fence");
    }

    // reset the command buffer
    m_command_buffers[m_current_frame].reset();

    // record the commands to render a frame
    record_command_buffer(m_command_buffers[m_current_frame], imageIndex);

    // SUBMIT the command buffer
    // also configures synchronisation
    vk::Semaphore waitSemaphores[] = {framebufferData.image_available_sem}; // which sems to wait on before work begins
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::CommandBuffer buffers[] = {m_command_buffers[m_current_frame]};
    vk::Semaphore signalSemaphores[] = {framebufferData.render_finish_sem};

    vk::SubmitInfo submitInfo(
        waitSemaphores,
        waitStages,
        buffers,
        signalSemaphores
    );

    m_graphics_queue.submit({submitInfo}, framebufferData.inflight_fence);

    // present to the swapchain
    vk::SwapchainKHR swapchains[] = {m_swapchain};
    uint32_t imageIndices[] = {imageIndex};
    vk::PresentInfoKHR presentInfo(signalSemaphores, swapchains, imageIndices);

    vk::Result presentResult = m_present_queue.presentKHR(presentInfo);
    // presentResult may be vk::Result::eSuboptimal but dont care
    // as we check and rebuild swapchain on next frame if so
    if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || m_resized) {
        m_resized = false;
        recreate_swapchain();
    } else if (presentResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present");
    }

    // run loading stuff here to simulate async loading
    // m_resource_loader->run_tasks();

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT; // go to the next frame origin
}


}