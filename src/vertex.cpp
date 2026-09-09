#include "vertex.hpp"

#include "command_buffers.hpp"

namespace wo_lume {


  Buffer createVertexBuffer(const GraphicDevice &deviceContext, const vk::raii::CommandPool &commandPool) {
    const vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    Buffer stagingBuffer = createBufferContext(
      deviceContext,
      bufferSize,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    void* data = stagingBuffer.memory.mapMemory(0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    stagingBuffer.memory.unmapMemory();

    Buffer vertexBuffer = createBufferContext(
      deviceContext,
      bufferSize,
      vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    copyBuffer(
      deviceContext,
      commandPool,
      stagingBuffer.buffer,
      vertexBuffer.buffer,
      bufferSize
    );

    return vertexBuffer;
  }


}

