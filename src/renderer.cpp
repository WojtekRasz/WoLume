#include "renderer.hpp"

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


  }
}
