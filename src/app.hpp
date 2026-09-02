#ifndef APP_HPP
#define APP_HPP

#include "instance.hpp"
#include "window.hpp"
#include "config.hpp"
#include "debug_messenger.hpp"
#include "device.hpp"

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

#include "swapchain.hpp"


namespace wo_lum {

  class App {
  public:
    App();

    void run() {
      mainLoop();
    }

  private:
    void initVulkan();
    void mainLoop();

    Window window;
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    DeviceContext deviceContext;
    SwapChainContext swapChainContext;
  };
}

#endif
