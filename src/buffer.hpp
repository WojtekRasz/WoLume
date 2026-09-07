#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan_raii.hpp>
#include "device.hpp"

namespace wo_lum{
  struct BufferContext {
    vk::raii::Buffer buffer = nullptr;
    vk::raii::DeviceMemory memory = nullptr;
    void *mapped = nullptr;
  };

  BufferContext createBufferContext(
    const DeviceContext &deviceContext,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties
  );
}



#endif //BUFFER_HPP
