#include "sampler.hpp"

namespace wo_lume {
  Sampler::Sampler(const GraphicDevice &device) {
    vk::PhysicalDeviceProperties properties = device.getPhysicalDevice().getProperties();
    vk::SamplerCreateInfo samplerInfo{
      .magFilter        = vk::Filter::eLinear,
      .minFilter        = vk::Filter::eLinear,
      .mipmapMode       = vk::SamplerMipmapMode::eLinear,
      .addressModeU     = vk::SamplerAddressMode::eRepeat,
      .addressModeV     = vk::SamplerAddressMode::eRepeat,
      .addressModeW     = vk::SamplerAddressMode::eRepeat,
      .anisotropyEnable = vk::True,
      .maxAnisotropy    = properties.limits.maxSamplerAnisotropy,
      .compareEnable    = vk::False,
      .compareOp        = vk::CompareOp::eAlways
    };

    sampler = vk::raii::Sampler(device.getLogicalDevice(), samplerInfo);
  }


} // wo_lume