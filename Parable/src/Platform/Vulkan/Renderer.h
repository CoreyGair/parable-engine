#pragma once

#include "Renderer/Renderer.h"

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Wrapper/VulkanWrapper.h"

#include "Asset/Handle.h"

class GLFWwindow;


namespace Parable
{
    class Mesh;
    class Texture;
}

namespace Parable::Vulkan
{


class Loader;
class MeshStore;
class TextureStore;

struct DrawCall
{
    Handle<Parable::Mesh> mesh;
    Handle<Parable::Texture> texture;
    glm::mat4 transform;
};

class Renderer : public Parable::Renderer
{
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

    Handle<Parable::Mesh> load_mesh(AssetDescriptor descriptor) override;

    Handle<Parable::Texture> load_texture(AssetDescriptor descriptor) override;

    void draw(Handle<Parable::Mesh> mesh, Handle<Parable::Texture> texture, glm::mat4& transform) override
    {
        m_draw_calls.push_back(DrawCall{mesh,texture,transform});
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
     * Descriptor set layout for per-material data.
     * 
     * atm, just binds a texture 
     */
    vk::DescriptorSetLayout m_material_descriptor_set_layout;
    /**
     * Descriptor set layout for per-object data, bound once per draw.
     */
    vk::DescriptorSetLayout m_draw_descriptor_set_layout;

    vk::DescriptorPool m_frame_descriptor_pool;
    vk::DescriptorPool m_material_descriptor_pool;
    vk::DescriptorPool m_draw_descriptor_pool;

    DescriptorSets m_frame_descriptor_sets;

    vk::PipelineLayout m_pipeline_layout;

    GraphicsPipeline m_graphics_pipeline;

    Framebuffers m_framebuffers;

    CommandPool m_command_pool;

    RecreatableImage m_depth_image;
    vk::ImageView m_depth_image_view;

    std::vector<Image> m_textures;
    std::vector<vk::ImageView> m_texture_views;
    std::vector<vk::Sampler> m_texture_samplers;
    std::vector<vk::DescriptorSet> m_texture_descriptor_sets;

    std::vector<Buffer> m_frame_uniform_buffers;

    std::vector<vk::CommandBuffer> m_command_buffers;

    UPtr<Loader> m_resource_loader;

    UPtr<MeshStore> m_mesh_store;
    UPtr<TextureStore> m_texture_store;

    std::vector<DrawCall> m_draw_calls;

    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    int m_current_frame = 0;

    bool m_resized = false;

    // temp, controls size of per-draw descriptor set pool
    // basically can only have this many meshes atm
    const size_t MAX_MESHES = 10;
};


}