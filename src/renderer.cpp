#include "renderer.hpp"

#include "command_buffers.hpp"
#include "config.hpp"
#include "debug_messenger.hpp"
#include "instance.hpp"
#include "pipeline.hpp"

namespace wo_lum {

  Renderer::Renderer(const Window &window) {
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



} // wo_lum