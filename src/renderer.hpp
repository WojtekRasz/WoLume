#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <vulkan/vulkan_raii.hpp>

#include "device.hpp"
#include "swapchain.hpp"

namespace wo_lum {
  class Renderer {
  public:
    explicit Renderer(const Window &window);

  private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    DeviceContext deviceContext;
    SwapChainContext swapChainContext;
    vk::raii::Pipeline pipeline = nullptr;
    vk::raii::CommandPool commandPool = nullptr;
    vk::raii::CommandBuffer commandBuffer = nullptr;

  };
} // wo_lum

#endif
