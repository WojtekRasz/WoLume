#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vulkan/vulkan_raii.hpp>

#include "graphic_device.hpp"
#include "window.hpp"

namespace wo_lume {

  struct SwapChainContext {
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    vk::SurfaceFormatKHR   surfaceFormat;
    vk::Extent2D           extent;
  };

  SwapChainContext createSwapChainContext(
    const GraphicDevice &deviceContext,
    const vk::raii::SurfaceKHR &surface,
    const Window &window
  );

}

#endif //SWAPCHAIN_HPP
