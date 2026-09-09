#ifndef INDEX_HPP
#define INDEX_HPP

#include <cstdint>
#include <vector>

#include "buffer.hpp"
#include "command_buffers.hpp"

namespace wo_lume {

  const std::vector<uint16_t> indices = {
    0, 1, 2, 0, 3, 1
  };

  Buffer createIndexBuffer(const GraphicDevice &deviceContext, const vk::raii::CommandPool &commandPool){
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    Buffer stagingBuffer = createBufferContext(
      deviceContext,
      bufferSize,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    void *data = stagingBuffer.memory.mapMemory(0, bufferSize);
    memcpy(data, indices.data(), (size_t) bufferSize);
    stagingBuffer.memory.unmapMemory();

    Buffer indexBuffer = createBufferContext(
      deviceContext,
      bufferSize,
      vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    copyBuffer(
      deviceContext,
      commandPool,
      stagingBuffer.buffer,
      indexBuffer.buffer,
      bufferSize
    );

    return indexBuffer;
  }

}

#endif //INDEX_HPP
