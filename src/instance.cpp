#include "instance.hpp"
#include "config.hpp"

namespace wo_lum {

  namespace {
    void printLayers(const std::vector<vk::LayerProperties>& layerPropertiesVector) {
      std::cout << "available layers:\n";
      for (const auto& layerProperties : layerPropertiesVector) {
        std::cout << '\t' << layerProperties.layerName << '\n';
      }
    }

    void printExtensions(const std::vector<vk::ExtensionProperties>& extensionPropertiesVector) {
      std::cout << "available extensions:\n";

      for (const auto& extensionProperties : extensionPropertiesVector) {
        std::cout << '\t' << extensionProperties.extensionName << '\n';
      }
    }

    void validateLayers(
      const std::vector<vk::LayerProperties>& availableLayersPropertiesVector,
      const std::vector<char const*> &requiredLayers
    ) {
      for (const auto& requiredLayer: requiredLayers){
        bool found = false;
        for (const auto& availableLayerProperties: availableLayersPropertiesVector) {
          if (strcmp(availableLayerProperties.layerName, requiredLayer) == 0) {
            found = true;
            break;
          }
        }
        if (!found){
          throw std::runtime_error("Required layer not supported: " + std::string(requiredLayer));
        }
      }
    }

    void validateExtensions(
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
          throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));
        }
      }
    }


  }



  vk::raii::Instance createInstance(
    const vk::raii::Context  &context,
    const std::vector<char const*> &requiredWindowExtensions
    ) {

    vk::ApplicationInfo appInfo = config::getAppInfo();

    //Vulkan layers
    auto availableLayersProperties = context.enumerateInstanceLayerProperties();
    std::vector<const char*> requiredLayers;
    if (config::enableValidationLayers) {
      requiredLayers = config::validationLayers;
    }
#if ENGINE_LOG_VULKAN_ENABLED
    printLayers(availableLayersProperties);
#endif

    validateLayers(availableLayersProperties, requiredLayers);

    //Extensions
    auto availableExtensionsProperties = context.enumerateInstanceExtensionProperties();

    std::vector<const char*> requiredExtensions = requiredWindowExtensions;
    if (config::enableValidationLayers) {
      requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

#if ENGINE_LOG_VULKAN_ENABLED
    printExtensions(availableExtensionsProperties);
#endif

    validateExtensions(availableExtensionsProperties, requiredExtensions);


    const vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
      .ppEnabledLayerNames = requiredLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
      .ppEnabledExtensionNames = requiredExtensions.data(),

    };

    return vk::raii::Instance{context, createInfo};
  }


} // my_vk_app