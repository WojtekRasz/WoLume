#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <vulkan/vulkan_raii.hpp>

#include "window_surface.hpp"

namespace wo_lume {

  class GraphicDevice {
  public:
    explicit GraphicDevice(const vk::raii::Instance &instance, const WindowSurface &windowSurface);

    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;


    [[nodiscard]] const vk::raii::Device& getLogicalDevice() const{ return device; }
    [[nodiscard]] const vk::raii::PhysicalDevice& getPhysicalDevice() const{ return physicalDevice; }
    [[nodiscard]] const vk::raii::Queue& getGraphicsQueue() const{ return graphicsQueue; }
    [[nodiscard]] uint32_t getGraphicsQueueFamilyIndex() const{ return graphicsQueueFamilyIndex; }

  private:
    static vk::raii::PhysicalDevice pickPhysicalDevice(const vk::raii::Instance &instance);
    [[nodiscard]] uint32_t findQueueFamilies(const vk::raii::SurfaceKHR &surface, vk::QueueFlagBits flagBits) const;
    [[nodiscard]] vk::raii::Device createLogicalDevice() const;

    vk::raii::PhysicalDevice physicalDevice{nullptr};
    vk::raii::Device device{nullptr};
    uint32_t graphicsQueueFamilyIndex = 0;
    vk::raii::Queue graphicsQueue{nullptr};
  };

}

#endif //DEVICE_HPP
