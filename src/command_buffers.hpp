#ifndef COMMAND_BUFFERS_HPP
#define COMMAND_BUFFERS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "graphic_device.hpp"

namespace wo_lume {

  vk::raii::CommandPool createCommandPool(const GraphicDevice &deviceContext);
  std::vector<vk::raii::CommandBuffer> createCommandBuffers(const GraphicDevice &deviceContext, const vk::raii::CommandPool &commandPool);
  void copyBuffer(
    const GraphicDevice &deviceContext,
    const vk::raii::CommandPool &commandPool,
    vk::raii::Buffer &srcBuffer,
    vk::raii::Buffer &dstBuffer,
    vk::DeviceSize size
  );

}

#endif //COMMAND_BUFFERS_HPP
