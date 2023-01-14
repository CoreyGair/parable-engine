#pragma once

#include "Render/Renderer.h"

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Wrapper/VulkanWrapper.h"

#include "MeshManager.h"


class GLFWwindow;


namespace Parable::Vulkan
{

namespace Vulkan { class Vertex; }


class Renderer : public Parable::Renderer
{
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

    MeshHandle load_mesh(std::string path) override
    {
        return m_mesh_manager.load_mesh(path);
    }
    void set_mesh_transform(MeshHandle handle, glm::mat4& transform) override
    {
        m_mesh_manager.set_mesh_transform(handle, transform);
    }

    void on_update() override;

    void on_resize(unsigned int width, unsigned int height) override { m_resized = true; }

private:

    void recreate_swapchain();

    void record_command_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

    GLFWwindow* m_window;

    vk::Instance m_instance;
    
    vk::SurfaceKHR m_surface;

    PhysicalDevice m_physical_device;

    Device m_device;

    vk::Queue m_graphics_queue;
    vk::Queue m_present_queue;
    vk::Queue m_transfer_queue;

    Swapchain m_swapchain;

    Renderpass m_renderpass;

    std::vector<vk::ShaderModule> m_shader_modules;
    
    // TODO: look into if we can use all 4 descriptor set bindings (maxDescriptorSetsBound, minimum guaranteed by standard is 4)
    //      current plan 3 bindings: global+per-frame, per-material, per-object/draw
    //      with 4 could have: global, per-frame, per-material, per-object/draw
    /**
     * Descriptor set layout for global/per-frame data, bound once per frame.
     */
    vk::DescriptorSetLayout m_frame_descriptor_set_layout;
    /**
     * Descriptor set layout for per-object data, bound once per draw.
     */
    vk::DescriptorSetLayout m_draw_descriptor_set_layout;

    vk::DescriptorPool m_frame_descriptor_pool;
    vk::DescriptorPool m_draw_descriptor_pool;

    DescriptorSets m_frame_descriptor_sets;

    vk::PipelineLayout m_pipeline_layout;

    GraphicsPipeline m_graphics_pipeline;

    Framebuffers m_framebuffers;

    CommandPool m_command_pool;

    RecreatableImage m_depth_image;
    vk::ImageView m_depth_image_view;

    Image m_texture_image;
    vk::ImageView m_texture_image_view;
    vk::Sampler m_texture_sampler;

    std::vector<Buffer> m_frame_uniform_buffers;

    std::vector<vk::CommandBuffer> m_command_buffers;

    MeshManager m_mesh_manager;

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    int m_current_frame = 0;

    bool m_resized = false;

    // temp, controls size of per-draw descriptor set pool
    // basically can only have this many meshes atm
    const size_t MAX_MESHES = 10;
};


}