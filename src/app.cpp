#include "app.hpp"

#include "command_buffers.hpp"
#include "device.hpp"
#include "pipeline.hpp"

namespace wo_lum {
  App::App():
    window{config::width, config::height, "Humble Window"}
  {
    initVulkan();
  }

  void App::initVulkan()
  {
    instance = createInstance(context, Window::getRequiredExtensions());
    if (config::enableValidationLayers) {
      debugMessenger = createDebugMessenger(instance);
    }
    surface = window.createSurface(instance);
    deviceContext = createDeviceContext(instance, surface);
    swapChainContext = createSwapChainContext(deviceContext, surface, window);
    pipeline = createGraphicsPipeline(deviceContext.device, swapChainContext);
    commandPool = createCommandPool(deviceContext);
    commandBuffer = createCommandBuffer(deviceContext, commandPool);
  }

  void App::mainLoop(){
    while (!window.shouldClose()) {
      Window::pollEvents();
    }
  }
}
