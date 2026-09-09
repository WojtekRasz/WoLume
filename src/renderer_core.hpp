#ifndef RENDERER_CORE_HPP
#define RENDERER_CORE_HPP
#include <vulkan/vulkan_raii.hpp>

#include "graphic_device.hpp"
#include "swapchain.hpp"
#include "window_surface.hpp"

namespace wo_lume {
  class RendererCore {
  public:
    explicit RendererCore(const Window &window);

    [[nodiscard]] const GraphicDevice& getGraphicDevice() const{ return graphicDevice; }
    [[nodiscard]] const SwapChain& getSwapChain() const{ return swapChain; }

  private:

    vk::raii::Context context;
    vk::raii::Instance instance;
    WindowSurface surface;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    GraphicDevice graphicDevice;
    SwapChain swapChain;
  };
}

#endif
