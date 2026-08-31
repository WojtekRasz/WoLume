#include "app.hpp"

#include "device.hpp"

namespace my_vk_app {
  void App::initVulkan() {
    instance = createInstance(context, window.getRequiredExtensions());

    if (config::enableValidationLayers) {
      debugMessenger = DebugMessenger(instance);
    }

    physicalDevice = pickPhysicalDevice(instance);
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies( physicalDevice );
    logicalDevice = createLogicalDevice(physicalDevice, queueFamilyIndices);
    graphicsQueue = getQueueHandle( logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0);
  }

  void App::mainLoop() {
    while (!window.shouldClose()) {
      window.pollEvents();
    }
  }
}
