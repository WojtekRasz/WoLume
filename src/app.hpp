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

#include "renderer.hpp"
#include "swapchain.hpp"


namespace wo_lum {

  class App {
  public:
    App();

    void run() {
      mainLoop();
    }

  private:
    void mainLoop();

    Window window;
    Renderer renderer;

  };
}

#endif
