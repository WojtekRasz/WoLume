#include "descriptor.hpp"

#include "config.hpp"
#include "uniform.hpp"

namespace wo_lume {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const GraphicDevice &deviceContext) {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{
      .binding = 0,
      .descriptorType = vk::DescriptorType::eUniformBuffer,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eVertex
    };

    const vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = 1,
      .pBindings = &uboLayoutBinding
    };

    return vk::raii::DescriptorSetLayout{deviceContext.getLogicalDevice(), layoutInfo};
  }

  vk::raii::DescriptorPool createDescriptorPool(const GraphicDevice &deviceContext){
    vk::DescriptorPoolSize poolSize{
      .type = vk::DescriptorType::eUniformBuffer,
      .descriptorCount = config::MAX_FRAMES_IN_FLIGHT
    };

    vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = config::MAX_FRAMES_IN_FLIGHT,
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize
    };

    return vk::raii::DescriptorPool{deviceContext.getLogicalDevice(), poolInfo};
  }

  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const GraphicDevice &deviceContext,
    const std::vector<Buffer> &uniformBuffers,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool
  ){
    std::vector<vk::DescriptorSetLayout> layouts(config::MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool     = descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
      .pSetLayouts        = layouts.data()
    };

    std::vector<vk::raii::DescriptorSet> descriptorSets = deviceContext.getLogicalDevice().allocateDescriptorSets(allocInfo);
    for (size_t i = 0; i < config::MAX_FRAMES_IN_FLIGHT; i++){
      vk::DescriptorBufferInfo bufferInfo{ .buffer = uniformBuffers[i].buffer, .offset = 0, .range = sizeof(UniformBufferObject) };

      vk::WriteDescriptorSet descriptorWrite{
        .dstSet          = descriptorSets[i],
        .dstBinding      = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &bufferInfo
      };

      deviceContext.getLogicalDevice().updateDescriptorSets(descriptorWrite, {});
    }

    return descriptorSets;
  }


}
