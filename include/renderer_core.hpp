#pragma once

#include "commomn.hpp"



struct RendererCore {
  SDL_Window* window;

  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;

  vk::raii::SurfaceKHR surface = nullptr;

  vk::raii::PhysicalDevice physical_device = nullptr;
  vk::raii::Device device = nullptr;
  uint32_t queue_family_index = -1;
  vk::raii::Queue queue = nullptr;

  vk::raii::SwapchainKHR swapchain = nullptr;

  RendererCore();
  ~RendererCore();

private:
  void init_instance();
  void init_window();
  void init_surface();
  void init_device_context();
  void init_swapchain();

  void destroy_window();
};
