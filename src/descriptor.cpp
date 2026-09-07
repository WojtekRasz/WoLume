#include "descriptor.hpp"

#include "config.hpp"
#include "uniform.hpp"

namespace wo_lum {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const DeviceContext &deviceContext) {
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

    return vk::raii::DescriptorSetLayout{deviceContext.device, layoutInfo};
  }

  vk::raii::DescriptorPool createDescriptorPool(const DeviceContext &deviceContext){
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

    return vk::raii::DescriptorPool{deviceContext.device, poolInfo};
  }

  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const DeviceContext &deviceContext,
    const std::vector<BufferContext> &uniformBuffers,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool
  ){
    std::vector<vk::DescriptorSetLayout> layouts(config::MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool     = descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
      .pSetLayouts        = layouts.data()
    };

    std::vector<vk::raii::DescriptorSet> descriptorSets = deviceContext.device.allocateDescriptorSets(allocInfo);
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

      deviceContext.device.updateDescriptorSets(descriptorWrite, {});
    }

    return descriptorSets;
  }


}
