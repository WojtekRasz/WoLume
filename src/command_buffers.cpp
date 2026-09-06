#include "command_buffers.hpp"

#include "config.hpp"
#include "device.hpp"

namespace wo_lum {


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

  void copyBuffer(
    const DeviceContext &deviceContext,
    const vk::raii::CommandPool &commandPool,
    vk::raii::Buffer &srcBuffer,
    vk::raii::Buffer &dstBuffer,
    const vk::DeviceSize size
  ){
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(deviceContext.device.allocateCommandBuffers(allocInfo).front());

    commandCopyBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();

    deviceContext.graphicsQueue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, nullptr);
    deviceContext.graphicsQueue.waitIdle();
  }



}

