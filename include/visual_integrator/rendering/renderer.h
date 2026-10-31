#pragma once

namespace visual_integrator {

class Renderer {
 public:
  // Requires a current OpenGL context that outlives this renderer.
  Renderer();
  ~Renderer();
  void render_frame(int framebuffer_width, int framebuffer_height) const;
  int funcImage();

  //private:
  unsigned int VAO, VBO, vertexShader, fragmentShader, shaderProgram;
};

}  // namespace visual_integrator
