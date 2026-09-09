#include "swapchain.hpp"

#include "graphic_device.hpp"


namespace wo_lume{
  namespace {
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
      const auto formatIt = std::ranges::find_if(
          availableFormats,
          [](const auto &format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
      return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
    }

    vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes){
      assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
      return std::ranges::any_of(availablePresentModes,
                                 [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
                 vk::PresentModeKHR::eMailbox :
                 vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities, const Window &window) {
      //
      if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return capabilities.currentExtent;
      }
      int width, height;
      glfwGetFramebufferSize(window.getGlfwWindow(), &width, &height);

      return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
      };
    }

    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities)
    {
      auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
      if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
      {
        minImageCount = surfaceCapabilities.maxImageCount;
      }
      return minImageCount;
    }


  }

  SwapChainContext createSwapChainContext(
    const GraphicDevice &deviceContext,
    const vk::raii::SurfaceKHR &surface,
    const Window &window
  ) {
    SwapChainContext swapChainContext;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = deviceContext.getPhysicalDevice().getSurfaceCapabilitiesKHR( *surface );
    swapChainContext.extent = chooseSwapExtent(surfaceCapabilities, window);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities) + 1;

    std::vector<vk::SurfaceFormatKHR> availableFormats = deviceContext.getPhysicalDevice().getSurfaceFormatsKHR( *surface );
    swapChainContext.surfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes = deviceContext.getPhysicalDevice().getSurfacePresentModesKHR( *surface );


    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface          = *surface,
      .minImageCount    = minImageCount,
      .imageFormat      = swapChainContext.surfaceFormat.format,
      .imageColorSpace  = swapChainContext.surfaceFormat.colorSpace,
      .imageExtent      = swapChainContext.extent,
      .imageArrayLayers = 1,
      .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform     = surfaceCapabilities.currentTransform,
      .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode      = chooseSwapPresentMode(availablePresentModes),
      .clipped          = true};

    swapChainContext.swapChain = vk::raii::SwapchainKHR( deviceContext.getLogicalDevice(), swapChainCreateInfo );
    swapChainContext.images = swapChainContext.swapChain.getImages();

    assert(swapChainContext.imageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo{
      .viewType         = vk::ImageViewType::e2D,
      .format           = swapChainContext.surfaceFormat.format,
      .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

    imageViewCreateInfo.components = {
      .r = vk::ComponentSwizzle::eIdentity,
      .g = vk::ComponentSwizzle::eIdentity,
      .b = vk::ComponentSwizzle::eIdentity,
      .a = vk::ComponentSwizzle::eIdentity
    };

    for (auto &image : swapChainContext.images)
    {
      imageViewCreateInfo.image = image;
      swapChainContext.imageViews.emplace_back( deviceContext.getLogicalDevice(), imageViewCreateInfo );
    }
    return swapChainContext;
  };
}



