#include "renderer_core.hpp"


namespace {
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

  vk::raii::Device create_device(const vk::raii::PhysicalDevice &physical_device, uint32_t queue_family_index) {
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

  void transition_image_layout(
    const vk::raii::CommandBuffer &command_buffer,
    vk::Image               &image,
    vk::ImageLayout         old_layout,
    vk::ImageLayout         new_layout,
    vk::AccessFlags2        src_access_mask,
    vk::AccessFlags2        dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask)
  {
    vk::ImageMemoryBarrier2 barrier = {
      .srcStageMask        = src_stage_mask,
      .srcAccessMask       = src_access_mask,
      .dstStageMask        = dst_stage_mask,
      .dstAccessMask       = dst_access_mask,
      .oldLayout           = old_layout,
      .newLayout           = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image               = image,
      .subresourceRange    = {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1}};
    vk::DependencyInfo dependency_info = {
      .dependencyFlags         = {},
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers    = &barrier};
    command_buffer.pipelineBarrier2(dependency_info);
  }
}



void RendererCore::init_instance(){
  vk::ApplicationInfo app_info{
    .pApplicationName = "Wolume App",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "WoLume Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_3
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

  if (! std::ranges::all_of(required_extensions, [&](std::string_view required) {
    return std::ranges::any_of(availableExtensionsProperties, [required](const auto& available) {
      return required == available.extensionName;
    });
  })) throw std::runtime_error("Required Vulkan extensions are not supported!");

  //Instance creation
  const vk::InstanceCreateInfo create_info{
    .pApplicationInfo = &app_info,
    .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
    .ppEnabledLayerNames = required_layers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
    .ppEnabledExtensionNames = required_extensions.data(),
  };

  instance = vk::raii::Instance{context, create_info};
}

void RendererCore::init_window() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    throw std::runtime_error(SDL_GetError());
  }

  window = SDL_CreateWindow(
    "WoLume",
    1280,
    720,
    SDL_WINDOW_VULKAN
  );

  if (window == nullptr) {
    throw std::runtime_error(SDL_GetError());
  }
}

void RendererCore::init_surface() {
  VkSurfaceKHR raw_surface{};

  if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &raw_surface)) {
    throw std::runtime_error(SDL_GetError());
  }

  surface = vk::raii::SurfaceKHR{instance, raw_surface};
}

void RendererCore::init_device_context() {
  physical_device = pick_physical_device(instance);
  queue_family_index = find_queue_family(physical_device, surface);
  device = create_device(physical_device, queue_family_index);
  queue = vk::raii::Queue(device, queue_family_index, 0);
}

void RendererCore::destroy_window() {
  if (window != nullptr) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  SDL_Quit();
}

void RendererCore::init_swapchain() {
  const auto surface_capabilities = physical_device.getSurfaceCapabilitiesKHR(*surface);

  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
    swapchain_extent = surface_capabilities.currentExtent;
  }
  else {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    swapchain_extent = vk::Extent2D{
      .width = std::clamp<uint32_t>(
        width,
        surface_capabilities.minImageExtent.width,
        surface_capabilities.maxImageExtent.width
      ),
      .height = std::clamp<uint32_t>(
        height,
        surface_capabilities.minImageExtent.height,
        surface_capabilities.maxImageExtent.height
      )
    };
  }

  uint32_t min_image_count = std::min(
    std::max(3u, surface_capabilities.minImageCount + 1),
    surface_capabilities.maxImageCount
  );

  auto available_formats = physical_device.getSurfaceFormatsKHR(*surface);
  const auto formatIt = std::ranges::find_if(available_formats, [](const auto &format) {
    return
      format.format == vk::Format::eB8G8R8A8Srgb &&
      format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
  });
  const auto format = (formatIt != available_formats.end()) ?
    *formatIt: // lambda true
    available_formats[0]; // lambda false

  swapchain_format = format.format;
  swapchain_color_space = format.colorSpace;

  auto available_present_modes = physical_device.getSurfacePresentModesKHR(*surface);
  assert(std::ranges::any_of(available_present_modes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
  auto present_mode = std::ranges::any_of(
    available_present_modes,
    [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }
    ) ?
    vk::PresentModeKHR::eMailbox: // lambda true
    vk::PresentModeKHR::eFifo; // lambda false


  vk::SwapchainCreateInfoKHR swapchain_create_info{
    .surface = *surface,
    .minImageCount = min_image_count,
    .imageFormat = swapchain_format,
    .imageColorSpace = swapchain_color_space,
    .imageExtent = swapchain_extent,
    .imageArrayLayers = 1,
    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode = vk::SharingMode::eExclusive,
    .preTransform = surface_capabilities.currentTransform,
    .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode = present_mode,
    .clipped = true
  };

  swapchain = vk::raii::SwapchainKHR(device, swapchain_create_info);

  swapchain_images = swapchain.getImages();

  vk::ImageViewCreateInfo image_view_create_info{
    .viewType         = vk::ImageViewType::e2D,
    .format           = swapchain_format,
    .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

  image_view_create_info.components = {
    .r = vk::ComponentSwizzle::eIdentity,
    .g = vk::ComponentSwizzle::eIdentity,
    .b = vk::ComponentSwizzle::eIdentity,
    .a = vk::ComponentSwizzle::eIdentity
  };

  for (const auto &image : swapchain_images){
    image_view_create_info.image = image;
    swapchain_image_views.emplace_back( device, image_view_create_info );
  }
}

void RendererCore::init_command_buffers() {
  vk::CommandPoolCreateInfo poolInfo{
    .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = queue_family_index
  };
  command_pool = vk::raii::CommandPool(device, poolInfo);

  const vk::CommandBufferAllocateInfo allocInfo{
    .commandPool = command_pool,
    .level = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = MAX_FRAMES_IN_FLIGHT
  };
  command_buffers = vk::raii::CommandBuffers{device, allocInfo};
}

void RendererCore::init_sync_objects() {
  for (size_t i = 0; i < swapchain_images.size(); i++){
    render_finished_semaphores.emplace_back(device, vk::SemaphoreCreateInfo());
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
    present_complete_semaphores.emplace_back(device, vk::SemaphoreCreateInfo());
    in_flight_fences.emplace_back(device, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
  }
}

const Frame& RendererCore::start_frame_rendering() {
  uint32_t frame_index = current_frame.frame_index;
  //std::cout << "Current frame:\n" << current_frame.frame_number << ", " << current_frame.frame_index << "\n";

  auto fence_result = device.waitForFences(
    *in_flight_fences[frame_index],
    vk::True,
    UINT64_MAX);
  if (fence_result != vk::Result::eSuccess){
    throw std::runtime_error("failed to wait for fence!");
  }
  device.resetFences(*in_flight_fences[frame_index]);

  auto [result, image_index] = swapchain.acquireNextImage(
    UINT64_MAX,
    *present_complete_semaphores[frame_index],
    nullptr);

  current_frame.swapchain_image_index = image_index;
  auto &current_command_buffer = command_buffers[frame_index];

  current_command_buffer.reset();
  current_command_buffer.begin({});
  transition_image_layout(
    current_command_buffer,
    swapchain_images[image_index],
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::eColorAttachmentOptimal,
    {},
    vk::AccessFlagBits2::eColorAttachmentReadNoncoherentEXT,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput
  );

  constexpr vk::ClearValue clearColor = vk::ClearColorValue(0.02f, 0.00f, 0.02f, 1.0f);
  vk::RenderingAttachmentInfo attachmentInfo = {
    .imageView   = swapchain_image_views[image_index],
    .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
    .loadOp      = vk::AttachmentLoadOp::eClear,
    .storeOp     = vk::AttachmentStoreOp::eStore,
    .clearValue  = clearColor
  };
  const vk::RenderingInfo renderingInfo = {
    .renderArea           = {
      .offset = {0, 0},
      .extent = swapchain_extent
    },
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &attachmentInfo
  };
  current_command_buffer.beginRendering(renderingInfo);

  current_command_buffer.setViewport(
      0,
      vk::Viewport(
        0.0f,
        0.0f,
        static_cast<float>(swapchain_extent.width),
        static_cast<float>(swapchain_extent.height),
        0.0f,
        1.0f
      )
    );
  current_command_buffer.setScissor(
    0,
    vk::Rect2D(vk::Offset2D(0, 0), swapchain_extent)
  );

  return current_frame;
}

void RendererCore::submit_frame_rendering(const Frame &frame){
  const auto &current_command_buffer = command_buffers[frame.frame_index];

  current_command_buffer.endRendering();
  transition_image_layout(
    current_command_buffer,
    swapchain_images[frame.swapchain_image_index],
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::ImageLayout::ePresentSrcKHR,
    vk::AccessFlagBits2::eColorAttachmentWrite,
    {},
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eBottomOfPipe
  );
  current_command_buffer.end();

  vk::PipelineStageFlags wait_destination_stage_mask(
  vk::PipelineStageFlagBits::eColorAttachmentOutput
  );

  const vk::SubmitInfo submitInfo{
    .waitSemaphoreCount   = 1,
    .pWaitSemaphores      = &*present_complete_semaphores[frame.frame_index],
    .pWaitDstStageMask    = &wait_destination_stage_mask,
    .commandBufferCount   = 1,
    .pCommandBuffers      = &*current_command_buffer,
    .signalSemaphoreCount = 1,
    .pSignalSemaphores    = &*render_finished_semaphores[frame.swapchain_image_index]
  };

  queue.submit(submitInfo, *in_flight_fences[frame.frame_index]);

  const vk::PresentInfoKHR presentInfoKHR{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores    = &*render_finished_semaphores[frame.swapchain_image_index],
    .swapchainCount     = 1,
    .pSwapchains        = &*swapchain,
    .pImageIndices      = &frame.swapchain_image_index
  };

  switch (auto result = queue.presentKHR(presentInfoKHR))
  {
    case vk::Result::eSuccess:
      break;
    case vk::Result::eSuboptimalKHR:
      std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
      break;
    default:
      break;        // an unexpected result is returned!
  }

  current_frame = {
    .frame_number = frame.frame_number + 1,
    .frame_index = (frame.frame_index + 1) % MAX_FRAMES_IN_FLIGHT
  };
}

RendererCore::RendererCore() {
  init_window();
  init_instance();
  init_surface();
  init_device_context();
  init_swapchain();
  init_command_buffers();
  init_sync_objects();
}

RendererCore::~RendererCore() {
  device.waitIdle();
  destroy_window();
}


