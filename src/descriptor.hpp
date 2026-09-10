#ifndef DESCRIPTORS_HPP
#define DESCRIPTORS_HPP

#include <vulkan/vulkan_raii.hpp>

#include "buffer.hpp"
#include "graphic_device.hpp"
#include "image.hpp"
#include "sampler.hpp"

namespace wo_lume {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const GraphicDevice &deviceContext);
  vk::raii::DescriptorPool createDescriptorPool(const GraphicDevice &deviceContext);
  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const GraphicDevice &deviceContext,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool,
    const std::vector<Buffer> &ubos,
    const std::vector<Image> &imageViews,
    const Sampler &sampler
  );
}

#endif //DESCRIPTORS_HPP
