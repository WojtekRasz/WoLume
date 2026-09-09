#include "renderer_core.hpp"

#include "config.hpp"
#include "debug_messenger.hpp"
#include "instance.hpp"

namespace wo_lume {
  RendererCore::RendererCore(const Window &window):
    instance(createInstance(context, Window::getRequiredExtensions())),
    debugMessenger(config::enableValidationLayers ? createDebugMessenger(instance) : nullptr),
    surface(instance, window),
    graphicDevice(instance, surface),
    swapChain(graphicDevice, surface, window)
  {}
}
