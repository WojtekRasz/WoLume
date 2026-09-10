#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "vertex.hpp"

#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>

#include "graphic_device.hpp"
#include "image.hpp"
#include "renderer_core.hpp"
#include "sampler.hpp"
#include "swapchain.hpp"

namespace wo_lume {
  class Renderer {
  public:
    explicit Renderer(const Window &window);
    ~Renderer();

    void recordCommandBuffer(uint32_t imageIndex) const;

    void drawFrame();

  private:
    void TransitionImageLayout(
      uint32_t                imageIndex,
      vk::ImageLayout         old_layout,
      vk::ImageLayout         new_layout,
      vk::AccessFlags2        src_access_mask,
      vk::AccessFlags2        dst_access_mask,
      vk::PipelineStageFlags2 src_stage_mask,
      vk::PipelineStageFlags2 dst_stage_mask
    ) const;
    void createSyncObjects();
    void updateUniformBuffer(uint32_t currentImage) const;

    RendererCore rendererCore;

    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline pipeline = nullptr;

    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::vector<Buffer> uniformBuffers;
    std::vector<Image> images;
    Sampler sampler;

    vk::raii::DescriptorPool descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> descriptorSets;

    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    uint32_t frameIndex = 0;
  };
} // wo_lum

#endif
