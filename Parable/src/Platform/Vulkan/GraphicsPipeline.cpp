#include "GraphicsPipeline.h"

#include "Core/Base.h"

#include "VulkanExceptions.h"
#include "GPU.h"
#include "Renderpass.h"


namespace Parable::Vulkan
{


GraphicsPipeline::GraphicsPipeline(
    GPU& gpu,
    VkPipelineLayout layout,
    Renderpass& renderpass,
    uint32_t subpass,
    PipelineCreateInfo& info
) : m_gpu(gpu), info(std::move(info)), m_subpass_index(subpass)
{
    create_pipeline(layout, renderpass, m_subpass_index);
}

void GraphicsPipeline::recreate_pipeline(VkPipelineLayout layout, Renderpass& renderpass)
{
    destroy_pipeline();

    create_pipeline(layout, renderpass, m_subpass_index);
}

void GraphicsPipeline::create_pipeline(VkPipelineLayout layout, Renderpass& renderpass, uint32_t subpass)
{
    // attach all the state which could change between pipeline creations
    info.color_blend_state.attachmentCount = info.color_blend_attachments.size();
    info.color_blend_state.pAttachments = info.color_blend_attachments.data();

    info.vertex_input_state.vertexBindingDescriptionCount = info.binding_descriptions.size();
    info.vertex_input_state.pVertexBindingDescriptions = info.binding_descriptions.data();

    info.vertex_input_state.vertexAttributeDescriptionCount = info.attribute_descriptions.size();
    info.vertex_input_state.pVertexAttributeDescriptions = info.attribute_descriptions.data();

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &info.viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &info.scissor;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // shader stages
    pipelineInfo.stageCount = info.shader_stages.size();
    pipelineInfo.pStages = info.shader_stages.data();
    // fixed function stages
    pipelineInfo.pVertexInputState = &info.vertex_input_state;
    pipelineInfo.pInputAssemblyState = &info.input_assembly_state;
    pipelineInfo.pViewportState = &viewport_state;
    pipelineInfo.pRasterizationState = &info.rasterizer_state;
    pipelineInfo.pMultisampleState = &info.multisampling_state;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &info.color_blend_state;
    pipelineInfo.pDynamicState = nullptr; // Optional
    // layout
    pipelineInfo.layout = layout;
    // render pass
    pipelineInfo.renderPass = renderpass.get_renderpass();
    pipelineInfo.subpass = subpass; // which subpass is this pipeline used on
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VkResult res = vkCreateGraphicsPipelines(m_gpu.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
    if (res != VK_SUCCESS) {
        throw VulkanFailedCreateException("graphics pipeline", res);
    }
}

void GraphicsPipeline::destroy_pipeline()
{
    vkDestroyPipeline(m_gpu.device, m_pipeline, nullptr);
}


// PIPELINE BUILDER

GraphicsPipelineBuilder::GraphicsPipelineBuilder(VkExtent2D initial_viewport_extent)
{
    // initial settings
    info.vertex_input_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    /**
     * Determines how geometry is created from the input verticies.
     */
    info.input_assembly_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, // we will specify triangles as a list, consecutive 3 elements = 1 tri
        .primitiveRestartEnable = VK_FALSE, // no 'primitive restart'
    };

    // defines what region of the framebuffer we output to
    // we want to draw to whole screen, so (0,0) to (extent.w, extent.h)
    info.viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)initial_viewport_extent.width,
        .height = (float)initial_viewport_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    
    // acts kind of like a filter, stuff outside the scissor is discarded
    // we want to render the whole image so the scissor is (0,0) to (extent.w, extent.h)
    info.scissor = {
        .offset = {0, 0},
        .extent = initial_viewport_extent,
    };

    // rasterizer: takes geometry from vert shader, turns into fragments, passes to frag shader
    info.rasterizer_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE, // if true, elements outside the near/far clip plane are clamped into range instead of discarded
        .rasterizerDiscardEnable = VK_FALSE, // disable the rasterizer
        .polygonMode = VK_POLYGON_MODE_FILL, // determines how the polys are drawn: we want to fill (can also do line or point)
        .cullMode = VK_CULL_MODE_BACK_BIT, // determines face culling: can cull back face, cull front, cull both or disable culling (show both sides)
        .frontFace = VK_FRONT_FACE_CLOCKWISE, // we specify vertices in clockwise order for front facing
        .depthBiasEnable = VK_FALSE, // can bias the depth value based on fragment slope
        .depthBiasConstantFactor = 0.0f, // Optional
        .depthBiasClamp = 0.0f, // Optional
        .depthBiasSlopeFactor = 0.0f, // Optional
        .lineWidth = 1.0f,
    };

    // sets up multisampling
    // enabling requires a device ext, not done yet
    info.multisampling_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f, // Optional
        .pSampleMask = nullptr, // Optional
        .alphaToCoverageEnable = VK_FALSE, // Optional
        .alphaToOneEnable = VK_FALSE, // Optional
    };
    
    // creates global state for color blending over all framebuffers
    // basically defines global variables for all color blending
    info.color_blend_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, // Optional
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }, // Optional
    };
}

std::unique_ptr<GraphicsPipeline> GraphicsPipelineBuilder::create(GPU& gpu, VkPipelineLayout layout, Renderpass& renderpass, uint32_t subpass)
{
    PipelineCreateInfo new_info = std::move(info);

    return std::make_unique<GraphicsPipeline>(gpu, layout, renderpass, subpass, new_info);
}


}