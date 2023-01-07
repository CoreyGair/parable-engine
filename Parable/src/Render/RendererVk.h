#pragma once

#include "Renderer.h"

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/VulkanWrapper.h"

class GLFWwindow;


namespace Parable 
{

namespace Vulkan { class Vertex; }


class RendererVk : public Renderer
{
public:
    RendererVk(GLFWwindow* window);
    ~RendererVk();

    void on_update() override;

    void on_resize(unsigned int width, unsigned int height) override { m_resized = true; }

private:

    void recreate_swapchain();

    void record_command_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

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

    std::vector<vk::ShaderModule> m_shader_modules;
    
    vk::DescriptorSetLayout m_descriptor_set_layout;

    vk::DescriptorPool m_descriptor_pool;

    Vulkan::DescriptorSets m_descriptor_sets;

    vk::PipelineLayout m_pipeline_layout;

    Vulkan::GraphicsPipeline m_graphics_pipeline;

    Vulkan::Framebuffers m_framebuffers;

    Vulkan::CommandPool m_command_pool;

    Vulkan::RecreatableImage m_depth_image;
    vk::ImageView m_depth_image_view;

    Vulkan::Image m_texture_image;
    vk::ImageView m_texture_image_view;
    vk::Sampler m_texture_sampler;

    std::vector<Vulkan::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    Vulkan::Buffer m_vertex_buffer;
    Vulkan::Buffer m_index_buffer;

    std::vector<Vulkan::Buffer> m_uniform_buffers;

    std::vector<vk::CommandBuffer> m_command_buffers;

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    int m_current_frame = 0;

    bool m_resized = false;
};


}