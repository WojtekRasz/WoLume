#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vulkan/vulkan_raii.hpp>

#include "device.hpp"
#include "window.hpp"

namespace wo_lum {

  struct SwapChainContext {
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    vk::SurfaceFormatKHR   surfaceFormat;
    vk::Extent2D           extent;
  };

  SwapChainContext createSwapChainContext(
    const DeviceContext &deviceContext,
    const vk::raii::SurfaceKHR &surface,
    const Window &window
  );

}

#endif //SWAPCHAIN_HPP
