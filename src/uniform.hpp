#ifndef UNIFORM_HPP
#define UNIFORM_HPP

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "graphic_device.hpp"

namespace wo_lume {

  struct UniformBufferObject{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  std::vector<Buffer> createUniformBuffers(const GraphicDevice &deviceContext);

}

#endif //UNIFORM_HPP
