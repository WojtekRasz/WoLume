#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include "graphic_device.hpp"

namespace wo_lume {

  class CommandBuffer;

  class Image {
  public:
    Image(
      const GraphicDevice &device,
      const CommandBuffer &commandBuffer,
      const std::string &textureFilename
    );

    [[nodiscard]] const vk::raii::ImageView& getImageView() const{return imageView;}
    [[nodiscard]] const vk::raii::Image& getImage() const{return image;}

  private:

    vk::raii::Image image = nullptr;
    vk::raii::DeviceMemory imageMemory = nullptr;
    vk::raii::ImageView imageView = nullptr;
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
  };
}

#endif
