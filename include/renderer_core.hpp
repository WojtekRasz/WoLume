#pragma once

#include "commomn.hpp"

struct RendererCore {
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  vk::raii::Device device = nullptr;
  vk::raii::Queue queue = nullptr;
};

RendererCore init_renderer_core();