#include "visual_integrator/graphics/renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <stdexcept>

namespace visual_integrator {

Renderer::Renderer() {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    throw std::runtime_error("Failed to load OpenGL");
  }
}

void Renderer::render_frame(int framebuffer_width, int framebuffer_height) const {
  glViewport(0, 0, framebuffer_width, framebuffer_height);
  glClear(GL_COLOR_BUFFER_BIT);
}

}  // namespace visual_integrator
