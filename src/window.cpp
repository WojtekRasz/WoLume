#include "window.hpp"

#include <stdexcept>

namespace wo_lume {

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

  void Window::pollEvents() {
    glfwPollEvents();
  }

  std::vector<const char*> Window::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
  }

  vk::raii::SurfaceKHR Window::createSurface(const vk::raii::Instance& instance) const {
    VkSurfaceKHR       _surface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
      throw std::runtime_error("failed to create window surface!");
    }
    return vk::raii::SurfaceKHR{instance, _surface};
  }


} // my_vk_app