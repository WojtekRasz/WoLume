#include "window.hpp"


void WindowContext::init_window() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    throw std::runtime_error(SDL_GetError());
  }

  window = SDL_CreateWindow(
    "WoLume",
    1280,
    720,
    SDL_WINDOW_VULKAN
  );

  if (window == nullptr) {
    throw std::runtime_error(SDL_GetError());
  }
}

void WindowContext::init_surface(const vk::raii::Instance &instance) {
  VkSurfaceKHR raw_surface{};

  if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &raw_surface)) {
    throw std::runtime_error(SDL_GetError());
  }

  surface = vk::raii::SurfaceKHR{instance, raw_surface};
}

void WindowContext::destroy_window() {
  if (window != nullptr) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  SDL_Quit();
}


