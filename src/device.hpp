#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <vulkan/vulkan_raii.hpp>

namespace my_vk_app {

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() const {
      return graphicsFamily.has_value();
    }
  };

  vk::raii::PhysicalDevice pickPhysicalDevice( const vk::raii::Instance &instance );
  QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice& physicalDevice);
  vk::raii::Device createLogicalDevice( const vk::raii::PhysicalDevice &physicalDevice, const QueueFamilyIndices& indices );
  vk::raii::Queue getQueueHandle( const vk::raii::Device& logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex = 0 );

}

#endif //DEVICE_HPP
