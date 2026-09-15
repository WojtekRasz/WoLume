#include "command_buffers.hpp"

#include "config.hpp"
#include "graphic_device.hpp"

namespace wo_lume {

  CommandBuffer::CommandBuffer(const GraphicDevice &device, const vk::raii::Queue &queue, vk::raii::CommandBuffer &&commandBuffer) :
    device(device),
    queue(queue),
    commandBuffer(std::move(commandBuffer))
  {}

  void CommandBuffer::begin() const { commandBuffer.begin({}); }
  void CommandBuffer::end() const { commandBuffer.end(); }

  void CommandBuffer::submit() const {
    queue.get().submit(
      vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandBuffer},
      nullptr
    );
  }

  void CommandBuffer::submitAndWait() const {
    queue.get().submit(
      vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandBuffer},
      nullptr
    );
    device.get().getGraphicsQueue().waitIdle();
  }

  void CommandBuffer::copyBufferToBuffer(
      const Buffer &srcBuffer,
      const Buffer &dstBuffer,
      const vk::DeviceSize size
  ) const {
    commandBuffer.copyBuffer(*srcBuffer.buffer, *dstBuffer.buffer, vk::BufferCopy{.size = size});
  }

  void CommandBuffer::copyBufferToImage(
    const Buffer &buffer,
    const vk::raii::Image &image,
    const uint32_t width,
    const uint32_t height
  ) const {
    const vk::BufferImageCopy region{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1
      },
      .imageOffset = {0, 0, 0},
      .imageExtent = {width, height, 1}
    };

    commandBuffer.copyBufferToImage(
        *buffer.buffer,
        *image,
        vk::ImageLayout::eTransferDstOptimal,
        region
    );
  }

  void CommandBuffer::transitionImageLayout(
    const vk::raii::Image &image,
    const vk::ImageLayout oldLayout,
    const vk::ImageLayout newLayout
  ) const {
    vk::ImageMemoryBarrier barrier{
      .oldLayout           = oldLayout,
      .newLayout           = newLayout,
      .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
      .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
      .image               = image,
      .subresourceRange    = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1}
    };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal){
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

      sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal){
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

      sourceStage      = vk::PipelineStageFlagBits::eTransfer;
      destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else{
      throw std::invalid_argument("unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
  }

  CommandPool::CommandPool(const GraphicDevice &device) : device(device) {
    vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = device.getGraphicsQueueFamilyIndex()
    };

    commandPool = vk::raii::CommandPool{device.getLogicalDevice(), poolInfo};
  }

  CommandBuffer CommandPool::createCommandBuffer(const vk::raii::Queue &queue, const vk::CommandBufferLevel level) const {
    const vk::CommandBufferAllocateInfo allocInfo{
      .commandPool = commandPool,
      .level = level,
      .commandBufferCount = 1
    };

    return CommandBuffer{device, queue, std::move(vk::raii::CommandBuffers{device.get().getLogicalDevice(), allocInfo}.front())};
  }

  std::vector<CommandBuffer> CommandPool::createCommandBuffers(const vk::raii::Queue &queue, const vk::CommandBufferLevel level, const uint32_t count) const {
    const vk::CommandBufferAllocateInfo allocInfo{
      .commandPool = commandPool,
      .level = level,
      .commandBufferCount = count
    };
    auto vkCommandBuffers = vk::raii::CommandBuffers{device.get().getLogicalDevice(), allocInfo};
    std::vector<CommandBuffer> commandBuffers;

    for (int i = 0; i < count; ++i) {
      commandBuffers.emplace_back(device, queue, std::move(vkCommandBuffers[i]));
    }

    return commandBuffers;
  }


}

