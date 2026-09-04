#include "app.hpp"

#include "command_buffers.hpp"
#include "device.hpp"
#include "pipeline.hpp"

namespace wo_lum {
  App::App():
    window{config::width, config::height, "Humble Window"},
    renderer{window}
  {

  }

  void App::mainLoop(){
    while (!window.shouldClose()) {
      Window::pollEvents();
    }
  }
}
