#include "renderer.hpp"

Renderer::Renderer() {
  RenderingSystemBuilder builder{core};
  system = builder.build();
}

Renderer::~Renderer() {
  core.device.waitIdle();
}

void Renderer::run() {
  bool running = true;

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;
        default: break;
      }
    }

    Frame frame = core.start_frame_rendering();

    const auto &command_buffer = core.command_buffers[frame.frame_index];

    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics ,*system.pipeline);
    command_buffer.draw(3, 1,0, 0);

    core.submit_frame_rendering(frame);
  }
}
