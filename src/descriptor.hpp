#ifndef DESCRIPTORS_HPP
#define DESCRIPTORS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "graphic_device.hpp"

namespace wo_lume {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const GraphicDevice &deviceContext);
  vk::raii::DescriptorPool createDescriptorPool(const GraphicDevice &deviceContext);
  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const GraphicDevice &deviceContext,
    const std::vector<Buffer> &uniformBuffers,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool
  );

}

#endif //DESCRIPTORS_HPP
