#include "app.hpp"

#include <iostream>
#include <cstdlib>



int main()
{
  try
  {
    wo_lume::App app;
    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}