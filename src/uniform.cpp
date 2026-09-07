#include "uniform.hpp"

#include "buffer.hpp"
#include "config.hpp"

namespace wo_lum {

  std::vector<BufferContext> createUniformBuffers(const DeviceContext &deviceContext){
    std::vector<BufferContext> uniformBuffers;
    for (size_t i = 0; i < config::MAX_FRAMES_IN_FLIGHT; i++)
    {
      vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
      BufferContext uniformBuffer = createBufferContext(
        deviceContext,
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
      );
      uniformBuffer.mapped = uniformBuffer.memory.mapMemory(0, bufferSize);
      uniformBuffers.emplace_back(std::move(uniformBuffer));
    }

    return uniformBuffers;
  }
}
