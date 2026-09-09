#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "graphic_device.hpp"

namespace wo_lume {
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription()
    {
      return {.binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex};
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
      return {
        {
          {.location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, pos)},
          {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}
        }
      };
    }
  };

  const std::vector<Vertex> vertices = {
    {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.5f, 0.0f}},
    {{0.5f, 0.0f, 0.5f}, {0.5f, 0.0f, 1.0f}},
    {{0.5f, 0.0f, -0.5f}, {0.0f, 0.0f, 0.5f}},
    {{-0.5f, 0.0f, 0.5f}, {0.5f, 1.0f, 0.5f}}
  };

  Buffer createVertexBuffer(const GraphicDevice &deviceContext, const vk::raii::CommandPool &commandPool);
}




#endif //VERTEX_HPP
