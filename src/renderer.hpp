#ifndef RENDERER_HPP
#define RENDERER_HPP

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#include <vulkan/vulkan_raii.hpp>


#include "device.hpp"
#include "swapchain.hpp"

namespace wo_lum {
  class Renderer {
  public:
    explicit Renderer(const Window &window);
    ~Renderer();

    void recordCommandBuffer(uint32_t imageIndex) const;

    void drawFrame();

  private:
    void TransitionImageLayout(
      uint32_t                imageIndex,
      vk::ImageLayout         old_layout,
      vk::ImageLayout         new_layout,
      vk::AccessFlags2        src_access_mask,
      vk::AccessFlags2        dst_access_mask,
      vk::PipelineStageFlags2 src_stage_mask,
      vk::PipelineStageFlags2 dst_stage_mask
    ) const;
    void createSyncObjects();

    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    DeviceContext deviceContext;
    SwapChainContext swapChainContext;
    vk::raii::Pipeline pipeline = nullptr;

    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    uint32_t frameIndex = 0;
  };
} // wo_lum

#endif
