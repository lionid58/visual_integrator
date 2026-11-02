#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace visual_integrator {

class Renderer {
 public:
  // Requires a current OpenGL context that outlives this renderer.
  Renderer();
  ~Renderer();

  void render_frame() const;
  void SetSize(int framebuffer_width, int framebuffer_height);

  void SetVideoData();
  void BuildShader();
  void BuildViewMatrixs();


  private:
  /*
  // VAO - ?
  // VBO - vertices buffer object
  // EBO - element buffer object
  */
  unsigned int VAO, VBO, EBO, shaderProgram;
   int screenWidth, screenHeight, vertexCount = 0;
};
}  // namespace visual_integrator
