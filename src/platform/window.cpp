#include "visual_integrator/platform/window.h"

#define GLFW_INCLUDE_NONE
// #define Leonid loh
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

namespace visual_integrator {
namespace {

void log_glfw_error(int error_code, const char* description) {
  std::cerr << "GLFW error " << error_code << ": " << (description ? description : "unknown error")
            << '\n';
}

std::runtime_error glfw_error(const std::string& action) {
  const char* description = nullptr;
  const int error_code = glfwGetError(&description);
  if (error_code == GLFW_NO_ERROR) {
    return std::runtime_error(action);
  }
  return std::runtime_error(action + ": " + (description ? description : "unknown GLFW error"));
}

}  // namespace

Window::Window() {
  glfwSetErrorCallback(log_glfw_error);
  if (!glfwInit()) {
    throw glfw_error("Failed to initialize GLFW");
  }
  try {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    if (!mode) {
      throw glfw_error("Failed to get the primary monitor video mode");
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

    // Select GL version + let the backend select a GLSL version
    // this part I also copy from example:
    const char* glsl_version = nullptr;
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window_ = glfwCreateWindow(width, height, "integrator app", nullptr, nullptr);
    if (!window_) {
      throw glfw_error("Failed to create GLFW window with an OpenGL 3.3 context");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    // this code I copy from:
    // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); // ???
    ImGuiIO& io = ImGui::GetIO(); // frons and sth
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

  } catch (...) {
    if (window_) {
      glfwDestroyWindow(window_);
    }
    glfwTerminate();
    throw;
  }
}

Window::~Window() {
  if (window_) {
    // Cleanup GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
  }
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
