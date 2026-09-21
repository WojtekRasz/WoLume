#pragma once
#include "commomn.hpp"

struct WindowContext {
  SDL_Window *window = nullptr;
  vk::raii::SurfaceKHR surface = nullptr;

  void init_window();
  void init_surface(const vk::raii::Instance &instance);

  void destroy_window();
};
