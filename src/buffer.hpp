#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan_raii.hpp>
#include "graphic_device.hpp"

namespace wo_lume{
  struct Buffer {

    vk::raii::Buffer buffer = nullptr;
    vk::raii::DeviceMemory memory = nullptr;
    void *mapped = nullptr;
  };

  Buffer createBufferContext(
    const GraphicDevice &deviceContext,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties
  );
}



#endif //BUFFER_HPP
