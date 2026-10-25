#pragma once

#include <utility>

struct GLFWwindow;

namespace visual_integrator {

// Owns the application's single window and GLFW lifetime. Use on the main thread.
// The OpenGL context is current after construction.
class Window {
 public:
  Window();
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(Window&&) = delete;

  bool should_close() const;
  std::pair<int, int> framebuffer_size() const;
  void poll_events() const;
  void swap_buffers() const;

 private:
  GLFWwindow* window_ = nullptr;
};

}  // namespace visual_integrator
