#include "visual_integrator/platform/window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace visual_integrator {

Window::Window() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  try {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    if (!mode) {
      throw std::runtime_error("Failed to get the primary monitor video mode");
    }
    std::cout << "Primary monitor resolution: " << mode->width << 'x' << mode->height << '\n';

    // Work areas and window sizes use screen coordinates, not framebuffer pixels.
    int work_width = 0;
    int work_height = 0;
    glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &work_width, &work_height);
    if (work_width <= 0 || work_height <= 0) {
      work_width = mode->width;
      work_height = mode->height;
    }

    constexpr double WINDOW_FRACTION = 0.8;
    const int width = std::max(1, static_cast<int>(work_width * WINDOW_FRACTION));
    const int height = std::max(1, static_cast<int>(work_height * WINDOW_FRACTION));
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window_ = glfwCreateWindow(width, height, "integrator app", nullptr, nullptr);
    if (!window_) {
      throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
  } catch (...) {
    if (window_) {
      glfwDestroyWindow(window_);
    }
    glfwTerminate();
    throw;
  }
}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Window::should_close() const { return glfwWindowShouldClose(window_); }

std::pair<int, int> Window::framebuffer_size() const {
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window_, &width, &height);
  return {width, height};
}

void Window::poll_events() const { glfwPollEvents(); }
void Window::swap_buffers() const { glfwSwapBuffers(window_); }

}  // namespace visual_integrator
