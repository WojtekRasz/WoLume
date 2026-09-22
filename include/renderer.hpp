#pragma once
#include  "renderer_core.hpp"
#include "rendering_system.hpp"

class Renderer {
public:
  Renderer();

  void run();

private:
  const RendererCore core;
  RenderingSystem system;
};
