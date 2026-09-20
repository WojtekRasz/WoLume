#include "renderer_core.hpp"

namespace {
  vk::raii::Instance init_instance( const vk::raii::Context &context ){

    vk::ApplicationInfo app_info{
      "Wolume App",
      VK_MAKE_VERSION(1, 0, 0),
      "WoLume Engine",
      VK_MAKE_VERSION(1, 0, 0),
      VK_API_VERSION_1_3
    };

    //Layers validation
    auto availableLayersProperties = context.enumerateInstanceLayerProperties();
    std::vector<const char*> required_layers = {
      "VK_LAYER_KHRONOS_validation"
    };
    if (! std::ranges::all_of(required_layers, [&](std::string_view required) {
      return std::ranges::any_of(availableLayersProperties, [required](const auto& available) {
        return required == available.layerName;
      });
    })) throw std::runtime_error("Required Vulkan layers are not supported!");

    //Extension validation
    auto availableExtensionsProperties = context.enumerateInstanceExtensionProperties();

    uint32_t count;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
    std::vector<const char*> required_extensions{sdlExtensions, sdlExtensions + count};

    if ( std::ranges::all_of(required_extensions, [&](std::string_view required) {
      return std::ranges::any_of(availableExtensionsProperties, [required](const auto& available) {
        return required == available.extensionName;
      });
    })) throw std::runtime_error("Required Vulkan extensions are not supported!");

    //Instance creation
    vk::InstanceCreateInfo create_info{
      {},
      &app_info,
      static_cast<uint32_t>(required_layers.size()),
      required_layers.data(),
      static_cast<uint32_t>(required_extensions.size()),
      required_extensions.data(),
    };

    return vk::raii::Instance{context, create_info};
  }

  /**
   * @param physical_device - physical device
   *
   * @return true if the device is suitable for our needs
   */
  bool is_device_suitable(const vk::raii::PhysicalDevice &physical_device) {
    const bool supports_vulkan_1_3 = physical_device.getProperties().apiVersion >= vk::ApiVersion13;

    const auto queue_families = physical_device.getQueueFamilyProperties();
    const bool supports_graphics = std::ranges::any_of(queue_families, [](const auto& family) {
      return static_cast<bool>(family.queueFlags & vk::QueueFlagBits::eGraphics);
    });

    const auto available_device_extensions = physical_device.enumerateDeviceExtensionProperties();
    const std::vector<const char*> required_device_extensions = {
      vk::KHRSwapchainExtensionName
    };
    const bool supports_required_extensions = std::ranges::all_of(required_device_extensions, [&](std::string_view req) {
        return std::ranges::any_of(available_device_extensions, [req](const auto& avail) {
          return req == avail.extensionName;
        });
      });

    const auto availableDeviceFeatures = physical_device.template getFeatures2<
      vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    >();
    const bool supports_required_features =
      availableDeviceFeatures.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceFeatures2>().features.fillModeNonSolid &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceFeatures2>().features.wideLines &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceFeatures2>().features.geometryShader &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceFeatures2>().features.tessellationShader &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
      availableDeviceFeatures.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

    return supports_vulkan_1_3 && supports_graphics && supports_required_extensions && supports_required_features;
  }


  /**
   * @param physical_device - physical device to be stored
   *
   * @return score of the device
   */
  uint32_t device_score(const vk::raii::PhysicalDevice &physical_device) {
    if (!is_device_suitable(physical_device)) return 0;

    const auto device_properties = physical_device.getProperties();
    uint32_t score = 0;
    if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
      score += 1000;
    }

    return score;
  }

  /**
   * @param instance - current vulkan instance
   *
   * @return best physical device
   */
  vk::raii::PhysicalDevice pick_physical_device(const vk::raii::Instance& instance) {
    const auto available_devices = instance.enumeratePhysicalDevices();
    if (available_devices.empty()) throw std::runtime_error("Couldn't find any GPU supported by Vulkan!");

    std::map<uint32_t, vk::raii::PhysicalDevice, std::greater<uint32_t>> devices_ranking;
    for (const auto& device: available_devices) {
      if (is_device_suitable(device)) devices_ranking.insert({device_score(device), device});
    }

    if (devices_ranking.empty()) throw std::runtime_error("Couldn't find any suitable GPU for WoLume Engine requirements");
    return devices_ranking.begin()->second;
  }

  uint32_t find_queue_family(
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::SurfaceKHR& surface
  ) {
    const auto queue_families =
      physical_device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < queue_families.size(); ++i) {
      const bool supports_graphics =
        static_cast<bool>(
          queue_families[i].queueFlags &
          vk::QueueFlagBits::eGraphics
        );

      const bool supports_present =
        physical_device.getSurfaceSupportKHR(i, *surface);

      if (supports_graphics && supports_present) {
        return i;
      }
    }

    throw std::runtime_error("Couldn't find a queue family supporting graphics and presentation!");
  }

  vk::raii::Device init_device(const vk::raii::PhysicalDevice &physical_device, uint32_t queue_family_index) {
    float queuePriority = 0.5f;

    auto queueFamilyIt = std::ranges::find_if(physical_device.getQueueFamilyProperties(), [](const auto& queue_family) {
      return queue_family;
    });

    vk::DeviceQueueCreateInfo device_queue_create_info{
      .queueFamilyIndex = queue_family_index,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
    };

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> feature_chain = {
      {.features = {.samplerAnisotropy = true }},
      {.shaderDrawParameters = true},
      {
        .synchronization2 = true,
        .dynamicRendering = true
      },
      {.extendedDynamicState = true}
    };

    const std::vector<const char*> required_device_extensions = {
      vk::KHRSwapchainExtensionName
    };

    vk::DeviceCreateInfo device_create_info{
      .pNext = &feature_chain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &device_queue_create_info,
      .enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size()),
      .ppEnabledExtensionNames = required_device_extensions.data()
    };

    return vk::raii::Device{physical_device, device_create_info};
  }

}


RendererCore init_renderer_core(const vk::raii::SurfaceKHR& surface) {
  RendererCore core;

  core.instance = init_instance(core.context);
  core.physical_device = pick_physical_device(core.instance);
  core.queue_family_index = find_queue_family(core.physical_device, surface);
  core.device = init_device(core.physical_device, core.queue_family_index);
  core.queue = vk::raii::Queue(core.device, core.queue_family_index, 0);

  return std::move(core);
}
