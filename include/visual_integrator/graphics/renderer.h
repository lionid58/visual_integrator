#pragma once

namespace visual_integrator {

class Renderer {
 public:
  // Requires a current OpenGL context that outlives this renderer.
  Renderer();
  void render_frame(int framebuffer_width, int framebuffer_height) const;
};

}  // namespace visual_integrator
