#include "device.hpp"

#include <iostream>
#include <map>

#include "config.hpp"

namespace my_vk_app {

  namespace {
    bool validateExtensions(
      const std::vector<vk::ExtensionProperties>& availableExtensionsPropertiesVector,
      const std::vector<char const*> &requiredExtensions
    ) {
      for (const auto& requiredExtension: requiredExtensions){
        bool found = false;
        for (const auto& availableExtension: availableExtensionsPropertiesVector) {
          if (strcmp(availableExtension.extensionName, requiredExtension) == 0) {
            found = true;
            break;
          }
        }
        if (!found){
          return false;
        }
      }
      return true;
    }

    bool validateQueueFamilies(const std::vector<vk::QueueFamilyProperties> &queueFamilies) {
      for (auto &queueFamilyProperties: queueFamilies) {
        if ( !!(queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics) ) {
          return true;
        }
      }
      return false;
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
  }

  vk::raii::PhysicalDevice pickPhysicalDevice(const vk::raii::Instance &instance) {
    auto physicalDevices = instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()){
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::multimap<uint32_t, vk::raii::PhysicalDevice> candidates;
    for (auto &physicalDevice : physicalDevices){
      uint32_t score = deviceScore(physicalDevice);
      if (score > 0) {
        candidates.insert(std::make_pair(score, physicalDevice));
      }
    }

#if ENGINE_LOG_VULKAN_ENABLED

#endif

    //Jeżeli zznaleziono jakiegoś kandydata
    if (!candidates.empty()){
#if ENGINE_LOG_VULKAN_ENABLED
      std::cout << "Picked device: " << candidates.rbegin()->second.getProperties().deviceName << "\n";
#endif
      return candidates.rbegin()->second;
    }
    else{
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice& physicalDevice) {
    QueueFamilyIndices indices;
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
      if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.graphicsFamily = i;
        break;
      }
    }
    return indices;
  }

  vk::raii::Device createLogicalDevice( const vk::raii::PhysicalDevice &physicalDevice, const QueueFamilyIndices &queueFamilyIndices ) {
    if( !queueFamilyIndices.isComplete() ){
      throw std::runtime_error("trying to create logical device without queue family indices");
    }

    float queuePriority = 0.5f;

    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
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
      {.dynamicRendering = true},            // Enable dynamic rendering from Vulkan 1.3
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

  vk::raii::Queue getQueueHandle(
    const vk::raii::Device& logicalDevice,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex
  ) {
    return vk::raii::Queue{logicalDevice, queueFamilyIndex, queueIndex};
  }
}
