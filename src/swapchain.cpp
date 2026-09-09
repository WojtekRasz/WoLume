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

  SwapChain::SwapChain(const GraphicDevice &deviceContext, const WindowSurface &surface, const Window &window) {

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = deviceContext.getPhysicalDevice().getSurfaceCapabilitiesKHR( *surface.getVkSurfaceKhr() );

    extent = chooseSwapExtent(surfaceCapabilities, window);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities) + 1;

    std::vector<vk::SurfaceFormatKHR> availableFormats = deviceContext.getPhysicalDevice().getSurfaceFormatsKHR( *surface.getVkSurfaceKhr() );
    surfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes = deviceContext.getPhysicalDevice().getSurfacePresentModesKHR( *surface.getVkSurfaceKhr() );


    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface          = *surface.getVkSurfaceKhr(),
      .minImageCount    = minImageCount,
      .imageFormat      = surfaceFormat.format,
      .imageColorSpace  = surfaceFormat.colorSpace,
      .imageExtent      = extent,
      .imageArrayLayers = 1,
      .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform     = surfaceCapabilities.currentTransform,
      .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode      = chooseSwapPresentMode(availablePresentModes),
      .clipped          = true};

    swapChain = vk::raii::SwapchainKHR( deviceContext.getLogicalDevice(), swapChainCreateInfo );
    images = swapChain.getImages();

    assert(imageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo{
      .viewType         = vk::ImageViewType::e2D,
      .format           = surfaceFormat.format,
      .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

    imageViewCreateInfo.components = {
      .r = vk::ComponentSwizzle::eIdentity,
      .g = vk::ComponentSwizzle::eIdentity,
      .b = vk::ComponentSwizzle::eIdentity,
      .a = vk::ComponentSwizzle::eIdentity
    };

    for (auto &image : images)
    {
      imageViewCreateInfo.image = image;
      imageViews.emplace_back( deviceContext.getLogicalDevice(), imageViewCreateInfo );
    }
  };
}



