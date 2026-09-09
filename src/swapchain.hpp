#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vulkan/vulkan_raii.hpp>

#include "graphic_device.hpp"
#include "window.hpp"

namespace wo_lume {

  class SwapChain {
  public:
    SwapChain(
      const GraphicDevice &deviceContext,
      const WindowSurface &surface,
      const Window &window
    );

    [[nodiscard]] const vk::raii::SwapchainKHR& getVkSwapChain() const{ return swapChain; }
    [[nodiscard]] vk::Extent2D getExtent() const{ return extent; }
    [[nodiscard]] const vk::SurfaceFormatKHR& getSurfaceFormat() const { return surfaceFormat; }

    [[nodiscard]] size_t getImagesCount() const{ return images.size(); }
    [[nodiscard]] const vk::Image& getImage(const uint32_t index) const { return images[index]; }
    [[nodiscard]] const vk::raii::ImageView& getImageView(const uint32_t index) const { return imageViews[index]; }

  private:
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::Extent2D extent;
  };
}

#endif //SWAPCHAIN_HPP
