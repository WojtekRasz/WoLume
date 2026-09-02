#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace wo_lum {
  class Window {
  public:
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] bool shouldClose() const;
    static void pollEvents() ;

    static std::vector<const char*> getRequiredExtensions() ;
    [[nodiscard]] GLFWwindow* getGlfwWindow() const { return window; }

    [[nodiscard]] vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance) const;

  private:
    GLFWwindow* window{nullptr};
  };
} // my_vk_app

#endif
