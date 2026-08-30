#include "app.hpp"

namespace my_vk_app {
  void App::initVulkan() {
    auto extensions = window.getRequiredExtensions();

    instance = createInstance(context, extensions);

    if (config::enableValidationLayers) {
      debugMessenger = DebugMessenger(instance);
    }
  }

  void App::mainLoop() {
    while (!window.shouldClose()) {
      window.pollEvents();
    }
  }
}