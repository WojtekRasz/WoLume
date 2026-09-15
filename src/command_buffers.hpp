#ifndef COMMAND_BUFFERS_HPP
#define COMMAND_BUFFERS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "graphic_device.hpp"
#include "buffer.hpp"
#include "image.hpp"

namespace wo_lume {
  class CommandBuffer {
  public:
    CommandBuffer(
      const GraphicDevice &device,
      const vk::raii::Queue &queue,
      vk::raii::CommandBuffer &&commandBuffer
    );

    [[nodiscard]] const vk::raii::CommandBuffer& getVkCommandBuffer() const{ return commandBuffer; }

    void begin() const;
    void end() const;
    void submit() const;
    void submitAndWait() const;

    void copyBufferToBuffer(
      const Buffer &srcBuffer,
      const Buffer &dstBuffer,
      vk::DeviceSize size
    ) const;

    void copyBufferToImage(
      const Buffer &buffer,
      const vk::raii::Image &image,
      uint32_t width,
      uint32_t height
    ) const;

    void transitionImageLayout(
      const vk::raii::Image &image,
      vk::ImageLayout oldLayout,
      vk::ImageLayout newLayout
    ) const;

  private:
    friend class CommandPool;

    std::reference_wrapper<const GraphicDevice> device;
    std::reference_wrapper<const vk::raii::Queue> queue;
    vk::raii::CommandBuffer commandBuffer;
  };

  class CommandPool {
  public:
    explicit CommandPool(const GraphicDevice &device);
    [[nodiscard]] CommandBuffer createCommandBuffer(const vk::raii::Queue &queue, vk::CommandBufferLevel level) const;
    [[nodiscard]] std::vector<CommandBuffer> createCommandBuffers(const vk::raii::Queue &queue, vk::CommandBufferLevel level, uint32_t count) const;

  private:
    std::reference_wrapper<const GraphicDevice> device;
    vk::raii::CommandPool commandPool = nullptr;
  };

}

#endif //COMMAND_BUFFERS_HPP
