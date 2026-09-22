#include "rendering_system.hpp"

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
    vk::ShaderModuleCreateInfo create_info{
      .codeSize = code.size() * sizeof(char),
      .pCode = reinterpret_cast<const uint32_t*>( code.data() )
    };

    return vk::raii::ShaderModule{device, create_info};
  }
}

RenderingSystemBuilder::RenderingSystemBuilder(const RendererCore &core):
  core(core)
{}

std::pair<vk::raii::PipelineLayout, vk::raii::Pipeline> RenderingSystemBuilder::build_pipeline() const {
   auto shaderCode = readFile("../shaders/bin/triangle.spv");
    auto shaderModule = createShaderModule(shaderCode, core.device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule,  .pName = "fragMain" };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data()
    };

    /*
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
      .vertexBindingDescriptionCount   = 1,
      .pVertexBindingDescriptions      = &bindingDescription,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
      .pVertexAttributeDescriptions    = attributeDescriptions.data()
    };
    */
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
      .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::Viewport viewport{
      0.0f,
      0.0f,
      static_cast<float>(core.swapchain_extent.width),
      static_cast<float>(core.swapchain_extent.height),
      0.0f, 1.0f
    };

    vk::Rect2D scissor{
      vk::Offset2D{ .x = 0, .y = 0 },
      core.swapchain_extent
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
      .frontFace               = vk::FrontFace::eClockwise,
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
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0
    };
    pipelineLayout = vk::raii::PipelineLayout(core.device, pipelineLayoutInfo);

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
        .colorAttachmentCount = 1, .pColorAttachmentFormats = &core.swapchain_format
      }
    };

    vk::raii::Pipeline pipeline{core.device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()};

    return {std::move(pipelineLayout), std::move(pipeline)};
}

RenderingSystem RenderingSystemBuilder::build() const {
  auto pipeline_ctx = build_pipeline();
  return {
    .pipeline_layout = std::move(pipeline_ctx.first),
    .pipeline = std::move(pipeline_ctx.second)
  };
}
