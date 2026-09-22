#pragma once
#include  <fstream>

#include "commomn.hpp"
#include "renderer_core.hpp"


struct RenderingSystem {
  vk::raii::PipelineLayout pipeline_layout = nullptr;
  vk::raii::Pipeline pipeline = nullptr;
};

class RenderingSystemBuilder {
public:
  RenderingSystemBuilder(
    const RendererCore &core
  );

  [[nodiscard]] RenderingSystem build() const;

private:

  [[nodiscard]] std::pair<vk::raii::PipelineLayout, vk::raii::Pipeline> build_pipeline() const;

  const RendererCore &core;
};