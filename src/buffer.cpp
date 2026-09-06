#include "buffer.hpp"

#include "command_buffers.hpp"

namespace wo_lum {

  BufferContext createBufferContext(
    const DeviceContext &deviceContext,
    const vk::DeviceSize size,
    const vk::BufferUsageFlags usage,
    const vk::MemoryPropertyFlags properties
  ) {
    BufferContext bufferContext;
    const vk::BufferCreateInfo bufferInfo{
      .size        = size,
      .usage       = usage,
      .sharingMode = vk::SharingMode::eExclusive
    };

    bufferContext.buffer = vk::raii::Buffer{deviceContext.device, bufferInfo};

    const vk::MemoryRequirements memRequirements = bufferContext.buffer.getMemoryRequirements();

    const vk::MemoryAllocateInfo memoryAllocateInfo{
      .allocationSize  = memRequirements.size,
      .memoryTypeIndex = deviceContext.findMemoryType(
        memRequirements.memoryTypeBits,
        properties
      )
    };

    bufferContext.memory = vk::raii::DeviceMemory(deviceContext.device, memoryAllocateInfo);
    bufferContext.buffer.bindMemory( *bufferContext.memory, 0 );

    return bufferContext;
  }


}



