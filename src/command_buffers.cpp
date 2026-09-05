#include "command_buffers.hpp"

#include "config.hpp"
#include "device.hpp"

namespace wo_lum {

  namespace {

  }

  vk::raii::CommandPool createCommandPool(const DeviceContext &deviceContext) {
    vk::CommandPoolCreateInfo poolInfo{
      .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = deviceContext.graphicsQueueFamilyIndex
    };

    return vk::raii::CommandPool{deviceContext.device, poolInfo};
  }

  std::vector<vk::raii::CommandBuffer> createCommandBuffers(const DeviceContext &deviceContext, const vk::raii::CommandPool &commandPool) {
    vk::CommandBufferAllocateInfo allocInfo{
      .commandPool = commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = config::MAX_FRAMES_IN_FLIGHT
    };

    return vk::raii::CommandBuffers{deviceContext.device, allocInfo};
  }



}

