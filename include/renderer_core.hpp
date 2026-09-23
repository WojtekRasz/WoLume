#pragma once

#include "commomn.hpp"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

struct Frame {
  uint32_t frame_number = 0; //which frame from the start

  uint32_t frame_index = 0; // which frame of frames in flight
  uint32_t swapchain_image_index = std::numeric_limits<uint32_t>::max();
};

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

  std::vector<vk::raii::Semaphore> present_complete_semaphores;
  std::vector<vk::raii::Semaphore> render_finished_semaphores;
  std::vector<vk::raii::Fence> in_flight_fences;

  Frame current_frame = {
    .frame_number = 0,
    .frame_index = 0,
  };

  RendererCore();
  ~RendererCore();

  const Frame& start_frame_rendering();
  void submit_frame_rendering(const Frame &frame);

private:
  void init_instance();
  void init_window();
  void init_surface();
  void init_device_context();
  void init_swapchain();
  void init_command_buffers();
  void init_sync_objects();

  void destroy_window();
};
