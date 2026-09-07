#ifndef DESCRIPTORS_HPP
#define DESCRIPTORS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "device.hpp"

namespace wo_lum {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const DeviceContext &deviceContext);
  vk::raii::DescriptorPool createDescriptorPool(const DeviceContext &deviceContext);
  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const DeviceContext &deviceContext,
    const std::vector<BufferContext> &uniformBuffers,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool
  );

}

#endif //DESCRIPTORS_HPP
