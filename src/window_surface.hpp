#ifndef WINDOW_SURFACE_HPP
#define WINDOW_SURFACE_HPP

#include "window.hpp"

namespace wo_lume {
  class WindowSurface {
  public:
    explicit WindowSurface(const vk::raii::Instance& instance, const Window &window): surface(window.createSurface(instance)){};

    [[nodiscard]] const vk::raii::SurfaceKHR& getVkSurfaceKhr() const{ return surface; }
  private:
    vk::raii::SurfaceKHR surface;
  };
}

#endif
