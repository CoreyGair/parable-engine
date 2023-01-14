#pragma once

#include <glm/glm.hpp>

// hashing funcs for glm types
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.hpp>

#include <array>

namespace Parable::Vulkan
{


struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        // describes the attributes of each element of a binding
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{
            vk::VertexInputAttributeDescription(
                0, // location
                0, // binding
                vk::Format::eR32G32B32Sfloat,
                offsetof(Vertex, pos)
            ), 
            vk::VertexInputAttributeDescription(
                1, // location
                0, // binding
                vk::Format::eR32G32B32Sfloat,
                offsetof(Vertex, color)
            ),
            vk::VertexInputAttributeDescription(
                2, // location
                0, // binding
                vk::Format::eR32G32Sfloat,
                offsetof(Vertex, texCoord)
            )
        };

        return attributeDescriptions;
    }

    // required for hashing
    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }

};



} // namespace Parable::Vulkan

namespace std {
    template<> struct hash<Parable::Vulkan::Vertex> {
        size_t operator()(Parable::Vulkan::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}