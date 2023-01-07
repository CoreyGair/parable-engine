#include "RendererVk.h"

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

// stuff for texture importing
// probs temp here
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//

// obj file loader, again temp
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <vulkan/vulkan.hpp>
#include "Platform/Vulkan/VulkanWrapper.h"
#include "Platform/Vulkan/VulkanExceptions.h"

#include "Platform/Vulkan/Vertex.h"

#include <GLFW/glfw3.h>

#include "Window/Window.h"

#include <fstream>

namespace Parable
{


struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

RendererVk::RendererVk(GLFWwindow* window) : m_window(window)
{
    VkResult res; // TODO: remove

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
        Vulkan::PhysicalDevicePicker physicalDevicePicker;

        // needs geom shaders & anisotropy
        // TODO: optionally use anisotropy based on avail
        physicalDevicePicker.add_filter([](vk::PhysicalDevice& pd) {
            auto features = pd.getFeatures();
            return features.geometryShader && features.samplerAnisotropy;
        });

        // need all queues
        physicalDevicePicker.add_filter([&](vk::PhysicalDevice& pd) {
            return Vulkan::PhysicalDevice::get_optional_queue_family_indices_from_physical_device(pd, m_surface).is_complete();
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
            Vulkan::SwapChainSupportDetails support = Vulkan::PhysicalDevice::get_swapchain_support_details_from_physical_device(pd, m_surface);
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

        m_physical_device = Vulkan::PhysicalDevice(m_instance, physicalDevicePicker);
    }

    // get queue families
    Vulkan::QueueFamilyIndices queueFamilyIndices = m_physical_device.get_queue_family_indices(m_surface);

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
    
    m_device = Vulkan::Device(m_physical_device, deviceCreateInfo);

    // GET QUEUES
    m_graphics_queue = (*m_device).getQueue(queueFamilyIndices.graphics_family, 0);
    m_present_queue = (*m_device).getQueue(queueFamilyIndices.presentation_family, 0);
    m_transfer_queue = (*m_device).getQueue(queueFamilyIndices.transfer_family, 0);

    // CREATE SWAP CHAIN
    m_swapchain = Vulkan::Swapchain(m_device, m_physical_device, m_surface, window);

    PBL_CORE_ASSERT(m_swapchain.get_views().size() > 0);

    // CREATE RENDERPASS

    Vulkan::RenderpassBuilder renderpassBuilder;

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
    ), Vulkan::AttachmentFormat::CONSTANT);
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

    // CREATE DESCRIPTOR SETS

    std::vector<vk::DescriptorSetLayoutBinding> descSetLayoutBindings{
        vk::DescriptorSetLayoutBinding(
            0, // binding
            vk::DescriptorType::eUniformBuffer,
            1, // descriptorCount
            vk::ShaderStageFlagBits::eVertex,
            {}
        ),
        vk::DescriptorSetLayoutBinding(
            1,
            vk::DescriptorType::eCombinedImageSampler,
            1,
            vk::ShaderStageFlagBits::eFragment,
            {}
        )
    };

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCreateInfo(
        {},
        descSetLayoutBindings
    );

    m_descriptor_set_layout = (*m_device).createDescriptorSetLayout(descSetLayoutCreateInfo);

    // CREATE PIPELINE

    std::vector<vk::DescriptorSetLayout> descSetLayouts{m_descriptor_set_layout};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
        {},
        descSetLayouts
    );

    m_pipeline_layout = (*m_device).createPipelineLayout(pipelineLayoutInfo);

    // load shaders
    vk::ShaderModule vertShaderModule = Vulkan::load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\vert.spv");
    vk::ShaderModule fragShaderModule = Vulkan::load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\frag.spv");

    // push these to be destroyed only when we will never recreate the pipeline again
    // (atm when the Renderer is destroyed)
    m_shader_modules.push_back(vertShaderModule);
    m_shader_modules.push_back(fragShaderModule);

    Vulkan::GraphicsPipelineBuilder graphicsPipelineBuilder;
    
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

    graphicsPipelineBuilder.add_binding_description(Vulkan::Vertex::getBindingDescription());

    auto attachment_descs = Vulkan::Vertex::getAttributeDescriptions();
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

    m_command_pool = Vulkan::CommandPool(m_device, cmdPoolInfo);

    // CREATE depth image
    vk::ImageCreateInfo depthImgCreateInfo = vk::ImageCreateInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eD32Sfloat, // TODO: in future, check if this is avaliable or use suitable alternative
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
    m_depth_image = Vulkan::RecreatableImage(m_device, m_physical_device, depthImgCreateInfo);

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
    m_framebuffers = Vulkan::Framebuffers(m_device, m_swapchain.get_views(), {m_depth_image_view}, m_renderpass, m_swapchain.get_extent());

    // CREATE texture image
    // read in image
    int tex_width, tex_height, tex_channels;
    stbi_uc* pixels = stbi_load("D:\\parable-engine\\Parable\\src\\Render\\Textures\\viking_room.png", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    vk::DeviceSize image_size = tex_width * tex_height * 4;
    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    // set up staging buf
    Vulkan::BufferBuilder textureStagingBufferBuilder;
    textureStagingBufferBuilder.buffer_info.size = image_size;
    textureStagingBufferBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    textureStagingBufferBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    textureStagingBufferBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Vulkan::Buffer textureStagingBuffer = textureStagingBufferBuilder.create(m_device, m_physical_device);

    // write tex to staging
    textureStagingBuffer.write(pixels, 0, image_size);

    // free texture
    stbi_image_free(pixels);

    // create image
    vk::ImageCreateInfo textureImageInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        vk::Extent3D(static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height), 1),
        1, // mipLevels
        1, // arrayLevels
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::SharingMode::eExclusive,
        {},
        vk::ImageLayout::eUndefined
    );

    m_texture_image = Vulkan::Image(m_device, m_physical_device, textureImageInfo);

    // now copy the staging buf into the texture
    // first have to transition it to transfer dst optimal layout, as that is what copyFromBuffer expects
    // need to use 2 single time command buffers as have to wait for transition to submit and fin before copying
    auto transitionCmd = m_command_pool.begin_single_time_command();
    m_texture_image.transition_layout(transitionCmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    transitionCmd.end_and_submit(m_graphics_queue);

    auto texCopyCmd = m_command_pool.begin_single_time_command();
    m_texture_image.copy_from_buffer(texCopyCmd, textureStagingBuffer, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
    texCopyCmd.end_and_submit(m_graphics_queue);

    // now can transition texture to shader optimal
    transitionCmd = m_command_pool.begin_single_time_command();
    m_texture_image.transition_layout(transitionCmd, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    transitionCmd.end_and_submit(m_graphics_queue);

    textureStagingBuffer.destroy();

    // CREATE texture image view
    m_texture_image_view = (*m_device).createImageView(
        vk::ImageViewCreateInfo(
            {},
            m_texture_image,
            vk::ImageViewType::e2D,
            vk::Format::eR8G8B8A8Srgb,
            {},
            vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor,
                0, // baseMipLevel
                1, // levelCount
                0, // baseArrayLayer
                1  // layerCount
            )
        )
    );

    // CREATE texture image sampler
    m_texture_sampler = (*m_device).createSampler(
        vk::SamplerCreateInfo(
            {},
            vk::Filter::eLinear,    // magFilter
            vk::Filter::eLinear,    // minFilter
            vk::SamplerMipmapMode::eLinear, // mipmapMode
            vk::SamplerAddressMode::eRepeat,    // addressModeU
            vk::SamplerAddressMode::eRepeat,    // addressModeV
            vk::SamplerAddressMode::eRepeat,    // addressModeW
            0.0f,   // mipLodBias
            true,   // anisotropyEnable
            (*m_physical_device).getProperties().limits.maxSamplerAnisotropy, // maxAnisotropy,
            false,  // compareEnable
            vk::CompareOp::eAlways,
            0.0f,   // minLod
            0.0f,   // maxLod
            vk::BorderColor::eIntOpaqueBlack,   // borderColor
            false   // unnormalizedCoordinates
        )
    );

    // LOAD model
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "D:\\parable-engine\\Parable\\src\\Render\\Models\\viking_room.obj")) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vulkan::Vertex, uint32_t> uniqueVertices{};
    // collect all the shapes to one vertices array
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vulkan::Vertex vertex{
                .pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                .color = {1.0f, 1.0f, 1.0f},
                .texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                }
            };
            
            if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                m_vertices.push_back(vertex);
            }

            m_indices.push_back(uniqueVertices[vertex]);
        }
    }

    // CREATE VERTEX BUFFER
    // use staging buffer to copy verts into device-local mem (faster memory)
    vk::DeviceSize vertexListSize = sizeof(m_vertices[0]) * m_vertices.size();

    Vulkan::BufferBuilder vertexStagingBufBuilder;
    vertexStagingBufBuilder.buffer_info.size = vertexListSize;
    vertexStagingBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    vertexStagingBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertexStagingBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Vulkan::Buffer vertexStagingBuffer = vertexStagingBufBuilder.create(m_device, m_physical_device);

    // write the verts into the staging buffer
    vertexStagingBuffer.write((void*)m_vertices.data(), 0, vertexListSize);

    Vulkan::BufferBuilder vertexBufBuilder;
    vertexBufBuilder.buffer_info.size = vertexListSize;
    vertexBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vertexBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    vertexBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    m_vertex_buffer = vertexBufBuilder.create(m_device, m_physical_device);

    // CREATE INDEX BUFFER
    // use staging buffer to copy indices into device-local mem (faster memory)
    VkDeviceSize indexListSize = sizeof(m_indices[0]) * m_indices.size();

    Vulkan::BufferBuilder indexStagingBufBuilder;
    indexStagingBufBuilder.buffer_info.size = indexListSize;
    indexStagingBufBuilder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferSrc;
    indexStagingBufBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    indexStagingBufBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Vulkan::Buffer indexStagingBuffer = indexStagingBufBuilder.create(m_device, m_physical_device);

    // write the verts into the staging buffer
    indexStagingBuffer.write((void*)m_indices.data(), 0, indexListSize);

    Vulkan::BufferBuilder index_buf_builder;
    index_buf_builder.buffer_info.size = indexListSize;
    index_buf_builder.buffer_info.usage  = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    index_buf_builder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    index_buf_builder.required_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

    m_index_buffer = index_buf_builder.create(m_device, m_physical_device);

    // copy from staging into vertex and indices
    // create a single time command buffer to do the transfer
    auto vertIndTransferCmdBuffer = m_command_pool.begin_single_time_command();
    m_vertex_buffer.copy_from(vertexStagingBuffer, vertIndTransferCmdBuffer);
    m_index_buffer.copy_from(indexStagingBuffer, vertIndTransferCmdBuffer);
    vertIndTransferCmdBuffer.end_and_submit(m_graphics_queue);

    vertexStagingBuffer.destroy();
    indexStagingBuffer.destroy();

    // CREATE UNIFORM BUFFERS

    vk::DeviceSize uniformBufferSize = sizeof(UniformBufferObject);

    m_uniform_buffers.reserve(MAX_FRAMES_IN_FLIGHT);

    Vulkan::BufferBuilder uniformBufferBuilder;

    uniformBufferBuilder.buffer_info.size = uniformBufferSize;
    uniformBufferBuilder.buffer_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    uniformBufferBuilder.buffer_info.sharingMode = vk::SharingMode::eExclusive;
    uniformBufferBuilder.required_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBufferBuilder.emplace_back_vector(m_uniform_buffers, m_device, m_physical_device);

        // keep persistently mapped
        m_uniform_buffers[i].map(0, uniformBufferSize);
    }

    // CREATE descriptor pool
    vk::DescriptorPoolSize descriptorPoolSizes[] = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        ),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        ),
    };

    vk::DescriptorPoolCreateInfo poolInfo(
        {},
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT), // maxSets
        descriptorPoolSizes
    );

    m_descriptor_pool = (*m_device).createDescriptorPool(poolInfo);

    // CREATE descriptor sets
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptor_set_layout);
    m_descriptor_sets = Vulkan::DescriptorSets(m_device, m_descriptor_pool, layouts);

    // populate descriptors
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vk::DescriptorBufferInfo uniformBufferInfo(
            m_uniform_buffers[i],
            0,
            sizeof(UniformBufferObject)
        );

        vk::DescriptorImageInfo texImageInfo(
            m_texture_sampler,
            m_texture_image_view,
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        vk::WriteDescriptorSet descriptorWrites[] = {
            vk::WriteDescriptorSet(
                m_descriptor_sets[i],
                0, // binding
                0, // arrayElement
                1, // descriptorCount
                vk::DescriptorType::eUniformBuffer,
                {},                     // pImageInfo
                &uniformBufferInfo,     // pBufferInfo
                {}                      // pTexelBufferView
            ),
            vk::WriteDescriptorSet(
                m_descriptor_sets[i],
                1, // binding
                0, // arrayElement
                1, // descriptorCount
                vk::DescriptorType::eCombinedImageSampler,
                &texImageInfo,  // pImageInfo
                {},             // pBufferInfo
                {}              // pTexelBufferView
            ),
        };

        (*m_device).updateDescriptorSets(descriptorWrites, {});
    }

    // CREATE COMMAND BUFFERS
    m_command_buffers = m_command_pool.create_command_buffers(vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
}

RendererVk::~RendererVk()
{
    // wait until the device has finished working
    (*m_device).waitIdle();

    m_framebuffers.destroy();
    m_swapchain.destroy();

    m_graphics_pipeline.destroy();
    (*m_device).destroyPipelineLayout(m_pipeline_layout);
    m_renderpass.destroy();

    for (auto& buf : m_uniform_buffers)
    {
        buf.destroy();
    }

    (*m_device).destroyDescriptorPool(m_descriptor_pool);

    (*m_device).destroySampler(m_texture_sampler);
    (*m_device).destroyImageView(m_texture_image_view);
    m_texture_image.destroy();

    (*m_device).destroyDescriptorSetLayout(m_descriptor_set_layout);

    m_index_buffer.destroy();
    m_vertex_buffer.destroy();

    m_command_pool.destroy();

    for (auto m : m_shader_modules)
    {
        (*m_device).destroyShaderModule(m);
    }

    m_device.destroy();

    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();
}

/**
 * Recreates the swapchain and all dependent components.
 * 
 * Useful when a property about the swapchain changes (e.g. window resize)
 */
void RendererVk::recreate_swapchain()
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

void RendererVk::record_command_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
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

    // command: bind the pipeling
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphics_pipeline);

    commandBuffer.bindVertexBuffers(0, {m_vertex_buffer}, {0});
    commandBuffer.bindIndexBuffer(m_index_buffer, 0, vk::IndexType::eUint32);

    // bind the descriptor set for this frame
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);

    // draw
    commandBuffer.drawIndexed(static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

    // end render pass
    commandBuffer.endRenderPass();

    // stop recording to buff
    commandBuffer.end();
}

/**
 * Called every frame, draw stuff
 */
void RendererVk::on_update()
{
    if (m_paused)
    {
        // dont draw if paused (usually when window minimised)
        return;
    }

    const Vulkan::FramebufferData& framebufferData = m_framebuffers[m_current_frame];

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

    // update uniforms
    // temp just to demo for now
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        // rotate model on y axis 90 degree per sec
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // looking down at 45 deg angle
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // perspective projection, 45 degree vertical fov
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain.get_extent().width / (float) m_swapchain.get_extent().height, 0.1f, 10.0f);

        // glm designed for opengl
        // opengl Y coord is inverted
        ubo.proj[1][1] *= -1;

        memcpy(m_uniform_buffers[m_current_frame].get_map(), &ubo, sizeof(ubo));
    }

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

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT; // go to the next frame origin
}


}