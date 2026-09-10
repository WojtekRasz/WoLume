#ifndef SAMPLER_HPP
#define SAMPLER_HPP
#include "graphic_device.hpp"

namespace wo_lume {
  class Sampler {
  public:
    explicit Sampler(const GraphicDevice &device);
    [[nodiscard]] const vk::raii::Sampler& getSampler() const {return sampler;}

  private:
    vk::raii::Sampler sampler = nullptr;
  };
} // wo_lume

#endif
