#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan_raii.hpp>
#include "swapchain.hpp"

namespace wo_lume {

  std::pair<vk::raii::PipelineLayout, vk::raii::Pipeline> createGraphicsPipeline(
    const vk::raii::Device &device,
    const SwapChain &swapChainContext,
    const vk::raii::DescriptorSetLayout & descriptorSetLayout
  );

}


#endif //PIPELINE_HPP
