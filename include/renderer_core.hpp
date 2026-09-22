#pragma once

#include "commomn.hpp"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

struct RendererCore {


  SDL_Window* window = nullptr;

  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;

  vk::raii::SurfaceKHR surface = nullptr;

  vk::raii::PhysicalDevice physical_device = nullptr;
  vk::raii::Device device = nullptr;
  uint32_t queue_family_index = -1;
  vk::raii::Queue queue = nullptr;

  vk::raii::SwapchainKHR swapchain = nullptr;
  vk::Extent2D swapchain_extent{};
  vk::Format swapchain_format{};
  vk::ColorSpaceKHR swapchain_color_space{};

  std::vector<vk::Image> swapchain_images;
  std::vector<vk::raii::ImageView> swapchain_image_views;

  vk::raii::CommandPool command_pool = nullptr;
  std::vector<vk::raii::CommandBuffer> command_buffers;

  RendererCore();
  ~RendererCore();

private:
  void init_instance();
  void init_window();
  void init_surface();
  void init_device_context();
  void init_swapchain();
  void init_command_buffers();

  void destroy_window();
};
