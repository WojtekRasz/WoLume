#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <vulkan/vulkan_raii.hpp>

#include <iostream>
#include <stdexcept>

namespace wo_lume {
  vk::raii::Instance createInstance(
    const vk::raii::Context  &context,
    const std::vector<char const*> &requiredWindowExtensions
  );
} // my_vk_app

#endif
