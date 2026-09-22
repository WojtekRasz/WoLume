#include "renderer.hpp"
#include "commomn.hpp"

int main() {
  Renderer renderer{};

  try {
    renderer.run();
  }
  catch (const std::exception &e) {
    std::cout << e.what();
  }

  return 0;
}
