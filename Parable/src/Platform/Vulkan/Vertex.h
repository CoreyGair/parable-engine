#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace Parable::Vulkan
{


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        // describes the attributes of each element of a binding
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0; // which binding the attr is for
        attributeDescriptions[0].location = 0; // the location index in the shader
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // format of data
        attributeDescriptions[0].offset = offsetof(Vertex, pos); // offset into struct

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

};


} // namespace Parable::Vulkan
