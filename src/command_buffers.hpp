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
  void endSingleTimeCommands(const GraphicDevice &device, vk::raii::CommandBuffer &&commandBuffer);
  vk::raii::CommandBuffer beginSingleTimeCommands(const GraphicDevice &device, const vk::raii::CommandPool &commandPool);

  void transitionImageLayout(
    vk::raii::CommandBuffer &commandBuffer,
    const vk::raii::Image &image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout
  );
  void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height);

}

#endif //COMMAND_BUFFERS_HPP
