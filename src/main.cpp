#include <exception>
#include <iostream>

#include "visual_integrator/app.h"

int main() {
  try {
    return visual_integrator::run();
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
