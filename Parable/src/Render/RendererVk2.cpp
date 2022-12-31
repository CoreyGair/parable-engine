#include "RendererVk2.h"

#include "pblpch.h"
#include "Core/Base.h"

// stuff for rotation code when updating mvp matrices
// temp
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
//

// stuff for texture importing
// probs temp here
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//

#include <vulkan/vulkan.hpp>
#include "Platform/Vulkan/VulkanWrapper.h"
#include "Platform/Vulkan/VulkanExceptions.h"

#include "Platform/Vulkan/Vertex.h"

#include <GLFW/glfw3.h>

#include "Window/Window.h"

#include <fstream>

namespace Parable
{


const std::vector<Vulkan::Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

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
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // add the glfw extensions to the other required extensions
    requiredDeviceExtensions.reserve(requiredDeviceExtensions.size() + glfwExtensionCount);
    for (size_t i = 0; i < glfwExtensionCount; ++i)
    {
        const char* glfwExt = glfwExtensions[i];
        if (find_if(
                requiredDeviceExtensions.cbegin(),
                requiredDeviceExtensions.cend(),
                [&glfwExt](const char* ext) { return strcmp(ext,glfwExt) == 0; }
            ) 
            == requiredDeviceExtensions.cend())
        {
            requiredDeviceExtensions.push_back(glfwExt);
        }
    }

    vk::InstanceCreateInfo instanceCreateInfo(
        {}, 
        &applicationInfo,            // app info
        validationLayers,           // enabled layers
        requiredDeviceExtensions    // enabled extensions
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

        // needs geom shaders
        physicalDevicePicker.add_filter([](vk::PhysicalDevice& pd) {
            return pd.getFeatures().geometryShader;
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
            VkDeviceSize max_mem_size = 0;
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
            return max_mem_size / 1000000;
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

    uint32_t colorSubpassIndex = renderpassBuilder.add_subpass(vk::SubpassDescription(
        {},
        vk::PipelineBindPoint::eGraphics,
        1,
        &renderpassBuilder.get_attachment_refs()[colorAttachmentRefIndex]
    ));

    renderpassBuilder.add_subpass_dependency(vk::SubpassDependency(
        VK_SUBPASS_EXTERNAL,    // src subpass, refer to the implicit subpasses before and after our actual one
        colorSubpassIndex,      // dest subpass, refers to our 1 and only subpass
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // wait on the swapchain to finish reading by waiting on the color attachment output
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {},                                         // src access mask
        vk::AccessFlagBits::eColorAttachmentWrite   // dst access mask
    ));

    m_renderpass = renderpassBuilder.create(m_device, m_swapchain);

    // CREATE DESCRIPTOR SETS

    std::vector<vk::DescriptorSetLayoutBinding> descSetLayoutBindings{
        vk::DescriptorSetLayoutBinding(
            0,
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eVertex,
            {}
        )
    };

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCreateInfo(
        {},
        descSetLayoutBindings
    );

    m_descriptor_set_layout = (*m_device).createDescriptorSetLayout(descSetLayoutCreateInfo);

    // CREATE PIPELINE

    Vulkan::PipelineLayoutBuilder layoutBuilder;

    std::vector<vk::DescriptorSetLayout> descSetLayouts{m_descriptor_set_layout};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
        {},
        descSetLayouts
    );

    m_vk_pipeline_layout = (*m_device).createPipelineLayout(pipelineLayoutInfo);

    // load shaders
    vk::ShaderModule vertShaderModule = Vulkan::load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\vert.spv");
    vk::ShaderModule fragShaderModule = Vulkan::load_shader(m_device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\frag.spv");

    // push these to be destroyed only when we will never recreate the pipeline again
    // (atm when the Renderer is destroyed)
    m_shader_modules.push_back(vertShaderModule);
    m_shader_modules.push_back(fragShaderModule);

    Vulkan::GraphicsPipelineBuilder graphicsPipelineBuilder(m_swapchain.get_extent());

    graphicsPipelineBuilder.info.inputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        VK_FALSE
    )

    graphicsPipelineBuilder.info.viewport.width = (float)m_swapchain.get_extent().width;
    graphicsPipelineBuilder.info.viewport.height = (float)m_swapchain.get_extent().height;
    graphicsPipelineBuilder.info.viewport.minDepth = 0.0f;
    graphicsPipelineBuilder.info.viewport.maxDepth = 1.0f;

    graphicsPipelineBuilder.info.scissor.extent = m_swapchain.get_extent();

    graphicsPipelineBuilder.info.rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo(
        {},
        VK_FALSE, // depthClampEnable,
        VK_FALSE, // rasterizerDiscardEnable
        vk::::PolygonMode::eFill,
        {}, // cullMode
        vk::CullModeFlags::eBackBit,
        vk::FrontFace::eCounterClockwise,
        VK_FALSE, // depthBiasEnable
        {}, // depthBiasConstFactor
        {}, // depthBiasClamp
        {}, // depthBiasSlopeFactor
        1.0f //lineWdith
    )

    graphicsPipelineBuilder.info.multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo(
        {},
        vk::SampleCountFlagBits::e1,
        VK_FALSE, // sampleShadingEnabled
    )

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

    graphicsPipelineBuilder.add_blend_attachment(vk::PipelineColorBlendAttachmentState(
        VK_TRUE,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eSrcOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR || vk::ColorComponentFlagBits::eG || vk::ColorComponentFlagBits::eB || vk::ColorComponentFlagBits::eA
    ));

    graphicsPipelineBuilder.add_binding_description(Vulkan::Vertex::getBindingDescription());

    auto attachment_descs = Vulkan::Vertex::getAttributeDescriptions();
    for (auto a : attachment_descs)
    {
        graphicsPipelineBuilder.add_attachment_description(std::move(a));
    }

    m_graphics_pipeline = graphicsPipelineBuilder.create(m_device, m_vk_pipeline_layout, m_renderpass, colorSubpassIndex);

    // CREATE FRAMEBUFFERS
    m_framebuffers = Vulkan::Framebuffers(m_device, m_swapchain->get_views(), m_renderpass, m_swapchain->get_extent());

    // CREATE COMMAND POOL
    VkCommandPoolCreateInfo cmd_pool_info{};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmd_pool_info.queueFamilyIndex = m_gpu->get_queue_family_indices().graphics_family.value();

    m_command_pool = std::make_unique<Vulkan::CommandPool>(*m_gpu, cmd_pool_info);

    // CREATE texture image
    // read in image
    int tex_width, tex_height, tex_channels;
    stbi_uc* pixels = stbi_load("D:\\parable-engine\\Parable\\src\\Render\\Textures\\texture.jpg", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    VkDeviceSize image_size = tex_width * tex_height * 4;
    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    // set up staging buf
    Vulkan::BufferBuilder texture_staging_buffer_builder;
    texture_staging_buffer_builder.buffer_info.size = image_size;
    texture_staging_buffer_builder.buffer_info.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    texture_staging_buffer_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    texture_staging_buffer_builder.required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    UPtr<Vulkan::Buffer> texture_staging_buffer = texture_staging_buffer_builder.create(*m_gpu);

    // write tex to staging
    texture_staging_buffer->write(pixels, 0, image_size);

    // free texture
    stbi_image_free(pixels);

    // create image
    VkImageCreateInfo texture_image_info{};
    texture_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    texture_image_info.imageType = VK_IMAGE_TYPE_2D;
    texture_image_info.extent.width = static_cast<uint32_t>(tex_width);
    texture_image_info.extent.height = static_cast<uint32_t>(tex_height);
    texture_image_info.extent.depth = 1;
    texture_image_info.mipLevels = 1;
    texture_image_info.arrayLayers = 1;
    texture_image_info.format = VK_FORMAT_R8G8B8A8_SRGB; // could fail, best to check and have multiple acceptable ones but this is commonly avaliable for now
    texture_image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    texture_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    texture_image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texture_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    texture_image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    texture_image_info.flags = 0; // Optional

    m_texture_image = std::make_unique<Vulkan::Image>(*m_gpu, &texture_image_info);

    // CREATE VERTEX BUFFER
    // use staging buffer to copy verts into device-local mem (faster memory)
    VkDeviceSize vertices_size = sizeof(vertices[0]) * vertices.size();

    Vulkan::BufferBuilder vertex_staging_buf_builder;
    vertex_staging_buf_builder.buffer_info.size = vertices_size;
    vertex_staging_buf_builder.buffer_info.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vertex_staging_buf_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vertex_staging_buf_builder.required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    UPtr<Vulkan::Buffer> vertex_staging_buffer = vertex_staging_buf_builder.create(*m_gpu);

    // write the verts into the staging buffer
    vertex_staging_buffer->write((void*)vertices.data(), 0, vertices_size);

    Vulkan::BufferBuilder vertex_buf_builder;
    vertex_buf_builder.buffer_info.size = vertices_size;
    vertex_buf_builder.buffer_info.usage  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertex_buf_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vertex_buf_builder.required_memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    m_vertex_buffer = vertex_buf_builder.create(*m_gpu);

    // CREATE INDEX BUFFER
    // use staging buffer to copy indices into device-local mem (faster memory)
    VkDeviceSize indices_size = sizeof(indices[0]) * indices.size();

    Vulkan::BufferBuilder index_staging_buf_builder;
    index_staging_buf_builder.buffer_info.size = indices_size;
    index_staging_buf_builder.buffer_info.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    index_staging_buf_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    index_staging_buf_builder.required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    UPtr<Vulkan::Buffer> index_staging_buffer = index_staging_buf_builder.create(*m_gpu);

    // write the verts into the staging buffer
    index_staging_buffer->write((void*)indices.data(), 0, indices_size);

    Vulkan::BufferBuilder index_buf_builder;
    index_buf_builder.buffer_info.size = indices_size;
    index_buf_builder.buffer_info.usage  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    index_buf_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    index_buf_builder.required_memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    m_index_buffer = index_buf_builder.create(*m_gpu);

    // CREATE UNIFORM BUFFERS

    VkDeviceSize uniform_buffer_size = sizeof(UniformBufferObject);

    m_uniform_buffers.reserve(MAX_FRAMES_IN_FLIGHT);

    Vulkan::BufferBuilder uniform_buffer_builder;

    uniform_buffer_builder.buffer_info.size = uniform_buffer_size;
    uniform_buffer_builder.buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    index_buf_builder.buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    uniform_buffer_builder.required_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniform_buffer_builder.emplace_back_vector(m_uniform_buffers, *m_gpu);

        // keep persistently mapped
        m_uniform_buffers[i].map(0, uniform_buffer_size);
    }

    // CREATE descriptor pool
    VkDescriptorPoolSize desc_pool_size{};
    desc_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    desc_pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &desc_pool_size;
    pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    m_descriptor_pool = std::make_unique<Vulkan::DescriptorPool>(*m_gpu, pool_info);

    // CREATE descriptor sets
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptor_set_layout->get_descriptor_set_layout());
    m_descriptor_sets = m_descriptor_pool->allocate_descriptor_sets(layouts);

    // populate descriptors
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = m_uniform_buffers[i].get_buffer();
        buffer_info.offset = 0;
        buffer_info.range = sizeof(UniformBufferObject);

        m_descriptor_sets->update_descriptor_set(i, 0, buffer_info);
    }

    // copy from staging into vertex and indices
    // create a tempoary command buffer to do the transfer
    VkCommandBuffer commandBuffer = m_command_pool->create_command_buffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1)[0];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // copy into vert buffer
    m_vertex_buffer->copy_from(*vertex_staging_buffer, commandBuffer);

    // copy into index buffer
    m_index_buffer->copy_from(*index_staging_buffer, commandBuffer);

    vkEndCommandBuffer(commandBuffer);
        
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_vk_graphics_queue); // wait for the transfer to complete

    // free temp buffer
    std::vector<VkCommandBuffer> bufs = {commandBuffer};
    m_command_pool->free_command_buffers(bufs);

    // CREATE COMMAND BUFFERS
    m_vk_command_buffers = m_command_pool->create_command_buffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, MAX_FRAMES_IN_FLIGHT);
}

RendererVk::~RendererVk()
{
    // wait until the device has finished working
    m_gpu->wait_for_idle();

    vkDestroyPipelineLayout(m_gpu->device, m_vk_pipeline_layout, nullptr);

    for (auto m : m_shader_modules)
    {
        vkDestroyShaderModule(m_gpu->device, m, nullptr);
    }
}

/**
 * Recreates the swapchain and all dependent components.
 * 
 * Useful when a property about the swapchain changes (e.g. window resize)
 */
void RendererVk::recreate_swapchain()
{
    PBL_CORE_TRACE("Vulkan: Swapchain recreated.");

    m_swapchain->recreate_swapchain(m_window);

    m_renderpass->recreate_renderpass(*m_swapchain);

    m_graphics_pipeline->recreate_pipeline(m_vk_pipeline_layout, *m_renderpass);

    m_framebuffers->recreate_framebuffers(m_swapchain->get_views(), *m_renderpass, m_swapchain->get_extent());
}

void RendererVk::record_command_buffer(VkCommandBuffer command_buffer, uint32_t imageIndex)
{
    // tel vulkan we are starting to record to a command buff
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0; // Optional
    begin_info.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // start a render pass
    VkRenderPassBeginInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = m_renderpass->get_renderpass();
    renderpass_info.framebuffer = (*m_framebuffers)[imageIndex].framebuffer;
    renderpass_info.renderArea.offset = {0, 0};
    renderpass_info.renderArea.extent = m_swapchain->get_extent();
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // what color should the color attatchment be cleared to
    renderpass_info.clearValueCount = 1;
    renderpass_info.pClearValues = &clearColor;

    // the render pass begins
    vkCmdBeginRenderPass(command_buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

    // command: bind the pipeling
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline->get_pipeline());

    VkBuffer vertex_buffers[] = {m_vertex_buffer->get_buffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

    vkCmdBindIndexBuffer(command_buffer, m_index_buffer->get_buffer(), 0, VK_INDEX_TYPE_UINT16);

    // bind the descriptor set for this frame
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vk_pipeline_layout, 0, 1, &(*m_descriptor_sets)[m_current_frame], 0, nullptr);

    // draw
    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    // end render pass
    vkCmdEndRenderPass(command_buffer);

    // stop recording to buff
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
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

    const Vulkan::FramebufferData& framebuffer_data = (*m_framebuffers)[m_current_frame];

    // wait for prev frame to finish
    vkWaitForFences(m_gpu->device, 1, &framebuffer_data.inflight_fence, VK_TRUE, UINT64_MAX);

    // aquire image from the swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_gpu->device, m_swapchain->get_swapchain(), UINT64_MAX, framebuffer_data.image_available_sem, VK_NULL_HANDLE, &imageIndex);
    
    // recreate the swap chain stuff if needed
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_resized) {
        m_resized = false;
        recreate_swapchain();
        return;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

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
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->get_extent().width / (float) m_swapchain->get_extent().height, 0.1f, 10.0f);

        // glm designed for opengl
        // opengl Y coord is inverted
        ubo.proj[1][1] *= -1;

        memcpy(m_uniform_buffers[m_current_frame].get_map(), &ubo, sizeof(ubo));
    }

    // reset the fence once we know we will render stuff
    vkResetFences(m_gpu->device, 1, &framebuffer_data.inflight_fence);

    // reset the command buffer
    vkResetCommandBuffer(m_vk_command_buffers[m_current_frame], 0);

    // record the commands to render a frame
    record_command_buffer(m_vk_command_buffers[m_current_frame], imageIndex);

    // SUBMIT the command buffer
    // also configures synchronisation
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {framebuffer_data.image_available_sem}; // which sems to wait on before work begins
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = waitStages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_vk_command_buffers[m_current_frame];
    VkSemaphore signalSemaphores[] = {framebuffer_data.render_finish_sem}; // which sem's to signal once the work is done
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_vk_graphics_queue, 1, &submit_info, framebuffer_data.inflight_fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // present to the swapchain
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signalSemaphores; // wait on the sem which is signalled when rendering finished
    VkSwapchainKHR swap_chains[] = {m_swapchain->get_swapchain()}; // we are presenting back to our swapchain
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &imageIndex; // this is the image to present
    present_info.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(m_vk_presentation_queue, &present_info);

    // recreate swap chain if needed
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_resized) {
        m_resized = false;
        recreate_swapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT; // go to the next frame origin
}


}