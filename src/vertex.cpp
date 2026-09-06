#include "vertex.hpp"

#include "command_buffers.hpp"

namespace wo_lum {


  BufferContext createVertexBuffer(const DeviceContext &deviceContext, const vk::raii::CommandPool &commandPool) {
    const vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    BufferContext stagingBuffer = createBufferContext(
      deviceContext,
      bufferSize,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    void* data = stagingBuffer.memory.mapMemory(0, bufferSize);
    memcpy(data, vertices.data(), bufferSize);
    stagingBuffer.memory.unmapMemory();

    BufferContext vertexBuffer = createBufferContext(
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

