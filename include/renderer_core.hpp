#pragma once

#include "commomn.hpp"

struct RendererCore {
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::PhysicalDevice physical_device = nullptr;
  vk::raii::Device device = nullptr;
  uint32_t queue_family_index = -1;
  vk::raii::Queue queue = nullptr;

  void init_instance();
  void init_device_context(const vk::raii::SurfaceKHR& surface);
};