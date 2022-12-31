#pragma once

#include "Renderer.h"

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/Device.h"
#include "Platform/Vulkan/PhysicalDevice.h"

class GLFWwindow;

namespace Parable::Vulkan
{
class GPU;
class Renderpass;
class GraphicsPipeline;
class Framebuffers;
class Buffer;
class CommandPool;
class DescriptorSetLayout;
class DescriptorPool;
class DescriptorSets;
class Image;
class Swapchain;

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

    vk::Instance m_instance;
    
    vk::SurfaceKHR m_surface;

    Vulkan::PhysicalDevice m_physical_device;

    Vulkan::Device m_device;

    vk::Queue m_graphics_queue;
    vk::Queue m_present_queue;
    vk::Queue m_transfer_queue;

    Vulkan::Swapchain m_swapchain;

    Vulkan::Renderpass m_renderpass;

    std::vector<VkShaderModule> m_shader_modules;
    
    vk::DescriptorSetLayout m_descriptor_set_layout;

    std::unique_ptr<Vulkan::DescriptorPool> m_descriptor_pool;

    std::unique_ptr<Vulkan::DescriptorSets> m_descriptor_sets;

    vk::PipelineLayout m_vk_pipeline_layout;

    Vulkan::GraphicsPipeline m_graphics_pipeline;

    Vulkan::Framebuffers m_framebuffers;

    Vulkan::CommandPool m_command_pool;

    std::unique_ptr<Vulkan::Image> m_texture_image;

    std::unique_ptr<Vulkan::Buffer> m_vertex_buffer;
    std::unique_ptr<Vulkan::Buffer> m_index_buffer;

    std::vector<Vulkan::Buffer> m_uniform_buffers;

    std::vector<VkCommandBuffer> m_vk_command_buffers;

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    int m_current_frame = 0;

    bool m_resized = false;
};


}