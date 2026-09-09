#include "app.hpp"

#include "command_buffers.hpp"
#include "graphic_device.hpp"
#include "pipeline.hpp"

namespace wo_lume {
  App::App():
    window{config::width, config::height, "Humble Window"},
    renderer{window}
  {

  }

  void App::mainLoop(){
    while (!window.shouldClose()) {
      Window::pollEvents();
      renderer.drawFrame();
    }
  }
}
