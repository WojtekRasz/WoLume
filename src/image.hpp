#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include "graphic_device.hpp"

namespace wo_lume {
  class Image {
  public:
    Image() = default;
    Image(
      const GraphicDevice &device,
      const vk::raii::CommandPool &commandPool,
      const std::string &textureFile
    );

    [[nodiscard]] const vk::raii::ImageView& getImageView() const{return imageView;}

  private:

    vk::raii::Image image = nullptr;
    vk::raii::DeviceMemory imageMemory = nullptr;
    vk::raii::ImageView imageView = nullptr;

  };
}

#endif
