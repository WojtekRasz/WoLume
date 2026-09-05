#include "device.hpp"

#include <iostream>
#include <map>

#include "config.hpp"

namespace wo_lum {

  //utils device functions
  namespace {
    bool validateExtensions(
      const std::vector<vk::ExtensionProperties>& availableExtensionsPropertiesVector,
      const std::vector<char const*> &requiredExtensions
    ) {
      return std::ranges::all_of(requiredExtensions, [&](std::string_view req) {
        return std::ranges::any_of(availableExtensionsPropertiesVector, [req](const auto& avail) {
          return req == avail.extensionName;
        });
      });
    }

    bool validateQueueFamilies(const std::vector<vk::QueueFamilyProperties> &queueFamilies) {
      return std::ranges::any_of(queueFamilies, [](const auto& family) {
        return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eGraphics);
      });
    }

    bool validateFeatures(const vk::raii::PhysicalDevice &physicalDevice) {
      auto features                 = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
                                                                       vk::PhysicalDeviceVulkan11Features,
                                                                       vk::PhysicalDeviceVulkan13Features,
                                                                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
      return features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
                                      features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                                      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
    }

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) {
      const bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

      const auto queueFamilies    = physicalDevice.getQueueFamilyProperties();
      const bool supportsGraphics = validateQueueFamilies(queueFamilies);

      const auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
      const bool supportsAllRequiredExtensions = validateExtensions(availableDeviceExtensions, config::requiredDeviceExtension);

      const bool supportsRequiredFeatures = validateFeatures(physicalDevice);

      return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
    }

    uint32_t deviceScore(const vk::raii::PhysicalDevice &physicalDevice) {
      if (!isDeviceSuitable(physicalDevice)) return 0;

      auto deviceProperties = physicalDevice.getProperties();
      uint32_t score = 0;
      if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
      }
      score += deviceProperties.limits.maxImageDimension2D;

      return score;
    }

    std::optional<uint32_t> findQueueFamilies(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR &surface) {
      auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

      std::optional<uint32_t> queueIndex = std::nullopt;
      for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
      {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
        {
          // found a queue family that supports both graphics and present
          queueIndex = qfpIndex;
          break;
        }
      }
      return queueIndex;
    }
  }

  //device creation functions
  namespace{
    vk::raii::PhysicalDevice pickPhysicalDevice(const vk::raii::Instance &instance) {
      auto physicalDevices = instance.enumeratePhysicalDevices();
      if (physicalDevices.empty()){
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
      }

      auto bestIt = std::ranges::max_element(physicalDevices, {}, [](const auto& device) {
        return deviceScore(device);
      });

      if (bestIt != physicalDevices.end() && deviceScore(*bestIt) > 0) {
        return *bestIt;
      }

      throw std::runtime_error("failed to find a suitable GPU!");
    }

    vk::raii::Device createLogicalDevice( const vk::raii::PhysicalDevice &physicalDevice, uint32_t queueFamilyIndices ) {
      float queuePriority = 0.5f;

      vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = queueFamilyIndices,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
      };

      vk::StructureChain<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
        {},                                    // vk::PhysicalDeviceFeatures2 (empty for now)
        {.shaderDrawParameters = true},        // Enable shader draw parameters from Vulkan 1.1
        {
          .synchronization2 = true,
          .dynamicRendering = true
        },            // Enable dynamic rendering from Vulkan 1.3
        {.extendedDynamicState = true}         // Enable extended dynamic state from the extension
      };

      vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(config::requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = config::requiredDeviceExtension.data()
      };

      return vk::raii::Device{physicalDevice, deviceCreateInfo};
    }
  }

  DeviceContext createDeviceContext(
    const vk::raii::Instance& instance,
    const vk::raii::SurfaceKHR& surface
  ) {
    DeviceContext deviceContext;

    deviceContext.physicalDevice = pickPhysicalDevice(instance);

    std::optional<uint32_t> _graphicsQueueFamilyIndex = findQueueFamilies(deviceContext.physicalDevice, surface);
    if ( !_graphicsQueueFamilyIndex.has_value() ) throw std::runtime_error("Cannot find graphics queue family index!");

    deviceContext.graphicsQueueFamilyIndex = _graphicsQueueFamilyIndex.value();
    deviceContext.device = createLogicalDevice(deviceContext.physicalDevice, _graphicsQueueFamilyIndex.value());
    deviceContext.graphicsQueue = vk::raii::Queue(deviceContext.device, deviceContext.graphicsQueueFamilyIndex, 0);

    return deviceContext;
  }

}
