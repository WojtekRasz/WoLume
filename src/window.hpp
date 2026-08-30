#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdint>

namespace my_vk_app {
  class Window {
  public:
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void pollEvents() const;

    std::vector<const char*> getRequiredExtensions() const;
    GLFWwindow* getNativeHandle() const { return window; }

  private:
    GLFWwindow* window{nullptr};
  };
} // my_vk_app

#endif
