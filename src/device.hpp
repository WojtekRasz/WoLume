#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <vulkan/vulkan_raii.hpp>

namespace wo_lum {

  struct DeviceContext {
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    uint32_t graphicsQueueFamilyIndex{0};
    vk::raii::Device device{nullptr};
    vk::raii::Queue graphicsQueue{nullptr};
  };

  DeviceContext createDeviceContext(
    const vk::raii::Instance& instance,
    const vk::raii::SurfaceKHR& surface
  );

}

#endif //DEVICE_HPP
