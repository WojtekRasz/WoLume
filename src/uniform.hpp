#ifndef UNIFORM_HPP
#define UNIFORM_HPP

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "device.hpp"

namespace wo_lum {

  struct UniformBufferObject{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  std::vector<BufferContext> createUniformBuffers(const DeviceContext &deviceContext);

}

#endif //UNIFORM_HPP
