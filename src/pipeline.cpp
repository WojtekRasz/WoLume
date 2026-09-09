#include "pipeline.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "swapchain.hpp"
#include "vertex.hpp"

namespace wo_lume{

  namespace {
    std::vector<char> readFile(const std::string& filename) {
      std::ifstream file(filename, std::ios::ate | std::ios::binary);

      if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
      }

      std::vector<char> buffer(file.tellg());

      file.seekg(0, std::ios::beg);
      file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

      file.close();

      return buffer;
    }

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, const vk::raii::Device &device){
      vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>( code.data() )
      };

      return vk::raii::ShaderModule{device, createInfo};
    }
  }


  std::pair<vk::raii::PipelineLayout, vk::raii::Pipeline> createGraphicsPipeline(
    const vk::raii::Device &device,
    const SwapChain &swapChainContext,
    const vk::raii::DescriptorSetLayout & descriptorSetLayout
  ) {
    auto shaderCode = readFile("../shaders/slang.spv");
    auto shaderModule = createShaderModule(shaderCode, device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule,  .pName = "fragMain" };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data()
    };

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
      .vertexBindingDescriptionCount   = 1,
      .pVertexBindingDescriptions      = &bindingDescription,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
      .pVertexAttributeDescriptions    = attributeDescriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
      .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(swapChainContext.getExtent().width),
      static_cast<float>(swapChainContext.getExtent().height),
      0.0f, 1.0f
    };

    vk::Rect2D scissor{
      vk::Offset2D{ .x = 0, .y = 0 },
      swapChainContext.getExtent()
    };

    vk::PipelineViewportStateCreateInfo viewportState{
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
      .depthClampEnable        = vk::False,
      .rasterizerDiscardEnable = vk::False,
      .polygonMode             = vk::PolygonMode::eFill,
      .cullMode                = vk::CullModeFlagBits::eBack,
      .frontFace               = vk::FrontFace::eCounterClockwise,
      .depthBiasEnable         = vk::False,
      .lineWidth               = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling{
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = vk::False
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable         = vk::True,
      .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
      .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
      .colorBlendOp        = vk::BlendOp::eAdd,
      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
      .alphaBlendOp        = vk::BlendOp::eAdd,
      .colorWriteMask = vk::ColorComponentFlagBits::eR |
                        vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB |
                        vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending{
      .logicOpEnable = vk::False,
      .logicOp = vk::LogicOp::eCopy,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment
    };

    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
      .setLayoutCount = 1,
      .pSetLayouts = &*descriptorSetLayout,
      .pushConstantRangeCount = 0
    };

    pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
      {
        .stageCount          = 2,
       .pStages             = shaderStages,
       .pVertexInputState   = &vertexInputInfo,
       .pInputAssemblyState = &inputAssembly,
       .pViewportState      = &viewportState,
       .pRasterizationState = &rasterizer,
       .pMultisampleState   = &multisampling,
       .pColorBlendState    = &colorBlending,
       .pDynamicState       = &dynamicState,
       .layout              = pipelineLayout,
       .renderPass          = nullptr
      },
      {
        .colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainContext.getSurfaceFormat().format
      }
    };

    vk::raii::Pipeline pipeline{device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()};

    return {std::move(pipelineLayout), std::move(pipeline)};
  }

}
