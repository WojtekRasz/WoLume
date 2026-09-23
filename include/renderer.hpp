#pragma once
#include  "renderer_core.hpp"
#include "rendering_system.hpp"

class Renderer {
public:
  Renderer();
  ~Renderer();

  void run();

private:
  RendererCore core;
  RenderingSystem system;
};
