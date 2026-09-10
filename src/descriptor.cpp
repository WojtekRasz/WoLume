#include "descriptor.hpp"

#include "config.hpp"
#include "uniform.hpp"

namespace wo_lume {

  vk::raii::DescriptorSetLayout createDescriptorSetLayout(const GraphicDevice &deviceContext) {
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
      {
        {
          .binding = 0,
          .descriptorType = vk::DescriptorType::eUniformBuffer,
          .descriptorCount = 1,
          .stageFlags = vk::ShaderStageFlagBits::eVertex
        },
        {
          .binding = 1,
          .descriptorType = vk::DescriptorType::eCombinedImageSampler,
          .descriptorCount = 1,
          .stageFlags = vk::ShaderStageFlagBits::eFragment
        }
      }
    };

    const vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data()
    };

    return vk::raii::DescriptorSetLayout{deviceContext.getLogicalDevice(), layoutInfo};
  }

  vk::raii::DescriptorPool createDescriptorPool(const GraphicDevice &deviceContext){
    std::array<vk::DescriptorPoolSize, 2> poolSize{
      {
        {
          .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = config::MAX_FRAMES_IN_FLIGHT
        },
        {
          .type = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount = config::MAX_FRAMES_IN_FLIGHT
        }
      }
    };

    vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = config::MAX_FRAMES_IN_FLIGHT,
      .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
      .pPoolSizes = poolSize.data()
    };

    return vk::raii::DescriptorPool{deviceContext.getLogicalDevice(), poolInfo};
  }

  std::vector<vk::raii::DescriptorSet> createDescriptorSets(
    const GraphicDevice &deviceContext,
    const vk::raii::DescriptorSetLayout &descriptorSetLayout,
    const vk::raii::DescriptorPool &descriptorPool,
    const std::vector<Buffer> &ubos,
    const std::vector<Image> &images,
    const Sampler &sampler
  ){
    std::vector<vk::DescriptorSetLayout> layouts(config::MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool     = descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
      .pSetLayouts        = layouts.data()
    };

    std::vector<vk::raii::DescriptorSet> descriptorSets = deviceContext.getLogicalDevice().allocateDescriptorSets(allocInfo);
    for (size_t i = 0; i < config::MAX_FRAMES_IN_FLIGHT; i++){
      vk::DescriptorBufferInfo bufferInfo{
        .buffer = ubos[i].buffer,
        .offset = 0,
        .range = sizeof(UniformBufferObject)
      };
      vk::DescriptorImageInfo  imageInfo{
        .sampler = sampler.getSampler(),
        .imageView = images[0].getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
      };


      std::array<vk::WriteDescriptorSet, 2> descriptorWrites{
        {
          {
            .dstSet = descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &bufferInfo
          },
        {
            .dstSet = descriptorSets[i],
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &imageInfo
          }
        }
      };

      deviceContext.getLogicalDevice().updateDescriptorSets(descriptorWrites, {});
    }

    return descriptorSets;
  }


}
