#include "command_buffers.hpp"

#include "config.hpp"
#include "graphic_device.hpp"

namespace wo_lume {


  vk::raii::CommandPool createCommandPool(const GraphicDevice &deviceContext) {
    vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
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
    const GraphicDevice &device,
    const vk::raii::CommandPool &commandPool,
    vk::raii::Buffer &srcBuffer,
    vk::raii::Buffer &dstBuffer,
    const vk::DeviceSize size
  ){
    vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands(device, commandPool);
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy{.size = size});
    endSingleTimeCommands(device, std::move(commandCopyBuffer));
  }

  vk::raii::CommandBuffer beginSingleTimeCommands(const GraphicDevice &device, const vk::raii::CommandPool &commandPool){
    vk::CommandBufferAllocateInfo allocInfo{.commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
    vk::raii::CommandBuffer       commandBuffer = std::move(vk::raii::CommandBuffers(device.getLogicalDevice(), allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return std::move(commandBuffer);
  }

  void endSingleTimeCommands(const GraphicDevice &device, vk::raii::CommandBuffer &&commandBuffer){
    commandBuffer.end();

    vk::SubmitInfo submitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandBuffer};
    device.getGraphicsQueue().submit(submitInfo, nullptr);
    device.getLogicalDevice().waitIdle();
  }

  void transitionImageLayout(
    vk::raii::CommandBuffer &commandBuffer,
    const vk::raii::Image &image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout
  ){
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

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

      sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

      sourceStage      = vk::PipelineStageFlagBits::eTransfer;
      destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
      throw std::invalid_argument("unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
  }

  void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height){
    vk::BufferImageCopy region{
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
        *buffer,
        *image,
        vk::ImageLayout::eTransferDstOptimal,
        region
    );
  }



}

