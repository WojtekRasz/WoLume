#include "command_buffers.hpp"

#include "config.hpp"
#include "graphic_device.hpp"

namespace wo_lume {


  vk::raii::CommandPool createCommandPool(const GraphicDevice &deviceContext) {
    vk::CommandPoolCreateInfo poolInfo{
      .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = deviceContext.getGraphicsQueueFamilyIndex()
    };

    return vk::raii::CommandPool{deviceContext.getLogicalDevice(), poolInfo};
  }

  std::vector<vk::raii::CommandBuffer> createCommandBuffers(const GraphicDevice &deviceContext, const vk::raii::CommandPool &commandPool) {
    vk::CommandBufferAllocateInfo allocInfo{
      .commandPool = commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = config::MAX_FRAMES_IN_FLIGHT
    };

    return vk::raii::CommandBuffers{deviceContext.getLogicalDevice(), allocInfo};
  }

  void copyBuffer(
    const GraphicDevice &deviceContext,
    const vk::raii::CommandPool &commandPool,
    vk::raii::Buffer &srcBuffer,
    vk::raii::Buffer &dstBuffer,
    const vk::DeviceSize size
  ){
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(deviceContext.getLogicalDevice().allocateCommandBuffers(allocInfo).front());

    commandCopyBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();

    deviceContext.getGraphicsQueue().submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, nullptr);
    deviceContext.getGraphicsQueue().waitIdle();
  }



}

