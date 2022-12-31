#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <array>

namespace Parable::Vulkan
{


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        // describes the attributes of each element of a binding
        std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{
            vk::VertexInputAttributeDescription(
                0, // location
                0, // binding
                vk::Format::eR32G32SFloat,
                offsetof(Vertex, pos)
            )
        };

        return attributeDescriptions;
    }

};


} // namespace Parable::Vulkan
