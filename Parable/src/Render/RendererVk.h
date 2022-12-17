#pragma once

#include "Renderer.h"

#include "pblpch.h"

#include <vulkan/vulkan.h>

class GLFWwindow;

namespace Parable::Vulkan
{
class GPU;
class Swapchain;
class Renderpass;
class GraphicsPipeline;
class Framebuffers;
class Buffer;
class CommandPool;
}

namespace Parable 
{


class RendererVk : public Renderer
{
public:
    RendererVk(GLFWwindow* window);
    ~RendererVk();

    void on_update() override;

    void on_resize(unsigned int width, unsigned int height) override { m_resized = true; }

private:

    void recreate_swapchain();

    void record_command_buffer(VkCommandBuffer command_buffer, uint32_t imageIndex);

    GLFWwindow* m_window;

    std::unique_ptr<Vulkan::GPU> m_gpu;

    VkQueue m_vk_graphics_queue = VK_NULL_HANDLE;

    VkQueue m_vk_presentation_queue = VK_NULL_HANDLE;

    VkQueue m_vk_transfer_queue = VK_NULL_HANDLE;

    std::unique_ptr<Vulkan::Swapchain> m_swapchain;

    std::unique_ptr<Vulkan::Renderpass> m_renderpass;

    std::vector<VkShaderModule> m_shader_modules;

    VkPipelineLayout m_vk_pipeline_layout = VK_NULL_HANDLE;

    std::unique_ptr<Vulkan::GraphicsPipeline> m_graphics_pipeline;

    std::unique_ptr<Vulkan::Framebuffers> m_framebuffers;

    std::unique_ptr<Vulkan::CommandPool> m_command_pool;

    std::unique_ptr<Vulkan::Buffer> m_vertex_buffer;
    std::unique_ptr<Vulkan::Buffer> m_index_buffer;

    std::vector<VkCommandBuffer> m_vk_command_buffers;

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    int m_current_frame = 0;

    bool m_resized = false;
};


}