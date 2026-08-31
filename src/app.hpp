#ifndef APP_HPP
#define APP_HPP

#include "instance.hpp"
#include "window.hpp"
#include "config.hpp"
#include "debug_messenger.hpp"

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>



namespace my_vk_app {

  class App {
  public:
    void run() {
      initVulkan();
      mainLoop();
    }

  private:
    void initVulkan();
    void mainLoop();

    Window window{config::width, config::height, config::appName};
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device logicalDevice = nullptr;
    vk::raii::Queue graphicsQueue = nullptr;
    DebugMessenger debugMessenger;
  };
}

#endif
