#include "image.hpp"

#include <stdexcept>
#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "command_buffers.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

namespace wo_lume {
  Image::Image(
    const GraphicDevice &device,
    const vk::raii::CommandPool &commandPool,
    const std::string &textureFile
  ){
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(textureFile.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels){
      throw std::runtime_error("failed to load texture image!");
    }

    Buffer stagingBuffer = createBufferContext(
        device,
        imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    void* data = stagingBuffer.memory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBuffer.memory.unmapMemory();

    stbi_image_free(pixels);

    vk::ImageCreateInfo imageInfo{
      .imageType   = vk::ImageType::e2D,
      .format      = vk::Format::eR8G8B8A8Srgb,
      .extent      = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1},
      .mipLevels   = 1,
      .arrayLayers = 1,
      .samples     = vk::SampleCountFlagBits::e1,
      .tiling      = vk::ImageTiling::eOptimal,
      .usage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      .sharingMode = vk::SharingMode::eExclusive
    };

    image = vk::raii::Image(device.getLogicalDevice(), imageInfo);

    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{
      .allocationSize  = memRequirements.size,
      .memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
    };
    imageMemory = vk::raii::DeviceMemory(device.getLogicalDevice(), allocInfo);
    image.bindMemory(imageMemory, 0);

    vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
    transitionImageLayout(commandBuffer, image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(commandBuffer, stagingBuffer.buffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(commandBuffer, image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    endSingleTimeCommands(device, std::move(commandBuffer));

    vk::ImageViewCreateInfo viewInfo{
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = vk::Format::eR8G8B8A8Srgb,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
      }
    };
    imageView = vk::raii::ImageView(device.getLogicalDevice(), viewInfo);
  }




} // wo_lume