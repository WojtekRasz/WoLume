#include "window.hpp"

#include <stdexcept>

namespace my_vk_app {

  Window::Window(uint32_t width, uint32_t height, const char* title) {
    if (!glfwInit()) {
      throw std::runtime_error("Nie udało się zainicjalizować GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
      glfwTerminate();
      throw std::runtime_error("Nie udało się utworzyć okna GLFW!");
    }
  }

  Window::~Window() {
    if (window) {
      glfwDestroyWindow(window);
    }
    glfwTerminate();
  }

  bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
  }

  void Window::pollEvents() const {
    glfwPollEvents();
  }

  std::vector<const char*> Window::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
  }


} // my_vk_app