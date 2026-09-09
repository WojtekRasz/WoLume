#include "renderer.hpp"

#include "descriptor.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "index.hpp"
#include "uniform.hpp"
#include "command_buffers.hpp"
#include "config.hpp"
#include "debug_messenger.hpp"
#include "instance.hpp"
#include "pipeline.hpp"


namespace wo_lume {

  Renderer::Renderer(const Window &window):
    instance(createInstance(context, Window::getRequiredExtensions())),
    debugMessenger(config::enableValidationLayers ? createDebugMessenger(instance) : nullptr),
    surface(instance, window),
    graphicDevice(instance, surface)
  {
    swapChainContext = createSwapChainContext(graphicDevice, surface.getVkSurfaceKhr(), window);
    descriptorSetLayout = createDescriptorSetLayout(graphicDevice);
    auto pipelineContext = createGraphicsPipeline(graphicDevice.getLogicalDevice(), swapChainContext, descriptorSetLayout);
    pipelineLayout = std::move(pipelineContext.first);
    pipeline = std::move(pipelineContext.second);
    commandPool = createCommandPool(graphicDevice);
    vertexBuffer = createVertexBuffer(graphicDevice, commandPool);
    indexBuffer = createIndexBuffer(graphicDevice, commandPool);
    uniformBuffers = createUniformBuffers(graphicDevice);
    descriptorPool = createDescriptorPool(graphicDevice);
    descriptorSets = createDescriptorSets(graphicDevice, uniformBuffers, descriptorSetLayout, descriptorPool);
    commandBuffers = createCommandBuffers(graphicDevice, commandPool);
    createSyncObjects();
  }

  Renderer::~Renderer() {
    // Wstrzymuje CPU do momentu, gdy GPU przetworzy wszystkie kolejki
    if (graphicDevice.getLogicalDevice() != nullptr) {
      graphicDevice.getLogicalDevice().waitIdle();
    }
  }


  void Renderer::recordCommandBuffer(const uint32_t imageIndex) const {
    auto &commandBuffer = commandBuffers[frameIndex];

    commandBuffer.begin({});

    TransitionImageLayout(
      imageIndex,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      {},                                                        // srcAccessMask (no need to wait for previous operations)
      vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
      vk::PipelineStageFlagBits2::eColorAttachmentOutput         // dstStage
    );

    constexpr vk::ClearValue clearColor = vk::ClearColorValue(0.02f, 0.00f, 0.02f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo = {
      .imageView   = swapChainContext.imageViews[imageIndex],
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp      = vk::AttachmentLoadOp::eClear,
      .storeOp     = vk::AttachmentStoreOp::eStore,
      .clearValue  = clearColor
    };

    const vk::RenderingInfo renderingInfo = {
      .renderArea           = {.offset = {0, 0}, .extent = swapChainContext.extent},
      .layerCount           = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments    = &attachmentInfo
    };

    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
    commandBuffer.bindVertexBuffers(0, *vertexBuffer.buffer, {0});
    commandBuffer.bindIndexBuffer(*indexBuffer.buffer, 0, vk::IndexType::eUint16);


    commandBuffer.setViewport(
      0,
      vk::Viewport(
        0.0f,
        static_cast<float>(swapChainContext.extent.height),
        static_cast<float>(swapChainContext.extent.width),
        -static_cast<float>(swapChainContext.extent.height),
        0.0f,
        1.0f
      )
    );
    commandBuffer.setScissor(
      0,
      vk::Rect2D(vk::Offset2D(0, 0), swapChainContext.extent)
    );

    commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipelineLayout,
      0,
      *descriptorSets[frameIndex],
      nullptr);
    commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    commandBuffer.endRendering();

    TransitionImageLayout(
      imageIndex,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::ePresentSrcKHR,
      vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
      {},                                                     // dstAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
      vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
    );

    commandBuffer.end();
  }

  void Renderer::drawFrame() {
    auto fenceResult = graphicDevice.getLogicalDevice().waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess)
    {
      throw std::runtime_error("failed to wait for fence!");
    }
    graphicDevice.getLogicalDevice().resetFences(*inFlightFences[frameIndex]);

    auto [result, imageIndex] = swapChainContext.swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

    updateUniformBuffer(frameIndex);

    commandBuffers[frameIndex].reset();
    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo   submitInfo{.waitSemaphoreCount   = 1,
                                      .pWaitSemaphores      = &*presentCompleteSemaphores[frameIndex],
                                      .pWaitDstStageMask    = &waitDestinationStageMask,
                                      .commandBufferCount   = 1,
                                      .pCommandBuffers      = &*commandBuffers[frameIndex],
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores    = &*renderFinishedSemaphores[imageIndex]};
    graphicDevice.getGraphicsQueue().submit(submitInfo, *inFlightFences[frameIndex]);

    const vk::PresentInfoKHR presentInfoKHR{.waitSemaphoreCount = 1,
                                            .pWaitSemaphores    = &*renderFinishedSemaphores[imageIndex],
                                            .swapchainCount     = 1,
                                            .pSwapchains        = &*swapChainContext.swapChain,
                                            .pImageIndices      = &imageIndex};
    result = graphicDevice.getGraphicsQueue().presentKHR(presentInfoKHR);
    switch (result)
    {
      case vk::Result::eSuccess:
        break;
      case vk::Result::eSuboptimalKHR:
        std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
        break;
      default:
        break;        // an unexpected result is returned!
    }

    frameIndex = (frameIndex + 1) % config::MAX_FRAMES_IN_FLIGHT;
  }

  void Renderer::createSyncObjects() {
    assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

    for (size_t i = 0; i < swapChainContext.images.size(); i++)
    {
      renderFinishedSemaphores.emplace_back(graphicDevice.getLogicalDevice(), vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < config::MAX_FRAMES_IN_FLIGHT; i++)
    {
      presentCompleteSemaphores.emplace_back(graphicDevice.getLogicalDevice(), vk::SemaphoreCreateInfo());
      inFlightFences.emplace_back(graphicDevice.getLogicalDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
    }
  }


  void Renderer::TransitionImageLayout(
    const uint32_t                imageIndex,
    const vk::ImageLayout         old_layout,
    const vk::ImageLayout         new_layout,
    const vk::AccessFlags2        src_access_mask,
    const vk::AccessFlags2        dst_access_mask,
    const vk::PipelineStageFlags2 src_stage_mask,
    const vk::PipelineStageFlags2 dst_stage_mask
  ) const {
    vk::ImageMemoryBarrier2 barrier = {
      .srcStageMask        = src_stage_mask,
      .srcAccessMask       = src_access_mask,
      .dstStageMask        = dst_stage_mask,
      .dstAccessMask       = dst_access_mask,
      .oldLayout           = old_layout,
      .newLayout           = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image               = swapChainContext.images[imageIndex],
      .subresourceRange    = {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1}};
    const vk::DependencyInfo dependency_info = {
      .dependencyFlags         = {},
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers    = &barrier
    };
    commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
  }

  void Renderer::updateUniformBuffer(uint32_t currentImage){
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(
      glm::radians(45.0f),
      static_cast<float>(swapChainContext.extent.width) / static_cast<float>(swapChainContext.extent.height),
      0.1f,
      10.0f
    );

    memcpy(uniformBuffers[currentImage].mapped, &ubo, sizeof(ubo));
  }


} // wo_lum