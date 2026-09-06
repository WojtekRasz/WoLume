#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <vulkan/vulkan_raii.hpp>

namespace wo_lum {

  struct DeviceContext {
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    uint32_t graphicsQueueFamilyIndex{0};
    vk::raii::Device device{nullptr};
    vk::raii::Queue graphicsQueue{nullptr};

    [[nodiscard]] uint32_t findMemoryType(const uint32_t typeFilter, const vk::MemoryPropertyFlags properties) const {
      const vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

      for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
          return i;
        }
      }
      throw std::runtime_error("failed to find suitable memory type!");
    }
  };

  DeviceContext createDeviceContext(
    const vk::raii::Instance& instance,
    const vk::raii::SurfaceKHR& surface
  );

}

#endif //DEVICE_HPP
