#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan_raii.hpp>
#include "swapchain.hpp"

namespace wo_lum {

  vk::raii::Pipeline createGraphicsPipeline(const vk::raii::Device &device, const SwapChainContext &swapChainContext);

}


#endif //PIPELINE_HPP
