#ifndef COMMAND_BUFFERS_HPP
#define COMMAND_BUFFERS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "device.hpp"

namespace wo_lum {

  vk::raii::CommandPool createCommandPool(const DeviceContext &deviceContext);
  std::vector<vk::raii::CommandBuffer> createCommandBuffers(const DeviceContext &deviceContext, const vk::raii::CommandPool &commandPool);

}

#endif //COMMAND_BUFFERS_HPP
