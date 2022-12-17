#include "RendererVk.h"

#include "pblpch.h"
#include "Core/Base.h"

#include <vulkan/vulkan.h>
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

RendererVk::RendererVk(GLFWwindow* window) : m_window(window)
{
    VkResult res;

    Vulkan::GPUBuilder gpu_builder;
    #ifdef PBL_DEBUG
        gpu_builder.validation_layers = std::vector<const char*> {
            "VK_LAYER_KHRONOS_validation"
        };
    #endif
    gpu_builder.device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    m_gpu = gpu_builder.create(window);

    // GET QUEUES
    const Vulkan::QueueFamilyIndices& queue_family_indices = m_gpu->get_queue_family_indices();
    vkGetDeviceQueue(m_gpu->device, queue_family_indices.graphics_family.value(), 0, &m_vk_graphics_queue);
    vkGetDeviceQueue(m_gpu->device, queue_family_indices.presentation_family.value(), 0, &m_vk_presentation_queue);
    vkGetDeviceQueue(m_gpu->device, queue_family_indices.transfer_family.value(), 0, &m_vk_transfer_queue);

    // CREATE SWAP CHAIN
    m_swapchain = std::make_unique<Vulkan::Swapchain>(*m_gpu, window);

    PBL_CORE_ASSERT(m_swapchain->get_views().size() > 0);

    // CREATE RENDERPASS

    Vulkan::RenderpassBuilder renderpass_builder;

    uint32_t colorAttachmentIndex = renderpass_builder.add_attachment({
        .format = m_swapchain->get_image_format(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // how are contents treated at start of pass (here, we want to clear it)
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // how are contents treated at end of pass (here, we store to mem)
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, 
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    });
    
    uint32_t colorAttachmentRefIndex = renderpass_builder.add_attachment_ref(colorAttachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    uint32_t colorSubpassIndex = renderpass_builder.add_subpass({
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &renderpass_builder.get_attachment_refs()[colorAttachmentRefIndex],
    });

    renderpass_builder.add_subpass_dependency({
        .srcSubpass = VK_SUBPASS_EXTERNAL, // refer to the implicit subpasses before and after our actual one
        .dstSubpass = colorSubpassIndex, // refers to our 1 and only subpass
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // wait on the swapchain to finish reading by waiting on the color attachment output
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    });

    m_renderpass = renderpass_builder.create(*m_gpu, *m_swapchain);

    // CREATE PIPELINE

    Vulkan::PipelineLayoutBuilder layoutBuilder;
    m_vk_pipeline_layout = layoutBuilder.create_layout(m_gpu->device);

    // load shaders
    VkShaderModule vertShaderModule = Vulkan::load_shader(m_gpu->device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\vert.spv");
    VkShaderModule fragShaderModule = Vulkan::load_shader(m_gpu->device, "D:\\parable-engine\\Parable\\src\\Render\\Shaders\\frag.spv");

    // push these to be destroyed only when we will never recreate the pipeline again
    // (atm when the Renderer is destroyed)
    m_shader_modules.push_back(vertShaderModule);
    m_shader_modules.push_back(fragShaderModule);

    Vulkan::GraphicsPipelineBuilder graphics_pipeline_builder(m_swapchain->get_extent());
    graphics_pipeline_builder.add_shader_stage({
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT, // this is a vertex shader stage
        .module = vertShaderModule,
        .pName = "main",
    });
    graphics_pipeline_builder.add_shader_stage({
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT, // this is a frag shader stage
        .module = fragShaderModule,
        .pName = "main",
    });

    graphics_pipeline_builder.add_blend_attachment({
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    });

    graphics_pipeline_builder.add_binding_description(Vulkan::Vertex::getBindingDescription());

    auto attachment_descs = Vulkan::Vertex::getAttributeDescriptions();
    for (auto a : attachment_descs)
    {
        graphics_pipeline_builder.add_attachment_description(std::move(a));
    }

    m_graphics_pipeline = graphics_pipeline_builder.create(*m_gpu, m_vk_pipeline_layout, *m_renderpass, colorSubpassIndex);

    // CREATE FRAMEBUFFERS
    m_framebuffers = std::make_unique<Vulkan::Framebuffers>(*m_gpu, m_swapchain->get_views(), *m_renderpass, m_swapchain->get_extent());

    // CREATE COMMAND POOL
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = m_gpu->get_queue_family_indices().graphics_family.value();

    m_command_pool = std::make_unique<Vulkan::CommandPool>(*m_gpu, pool_info);

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

    // draw
    vkCmdDraw(command_buffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
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