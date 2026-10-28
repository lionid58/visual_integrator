#include "visual_integrator/app.h"

#include "visual_integrator/platform/window.h"
#include "visual_integrator/graphics/renderer.h"

namespace visual_integrator {

int run() {
  Window window;
  Renderer renderer;
  while (!window.should_close()) {
    window.poll_events();
    const auto [width, height] = window.framebuffer_size();
    renderer.render_frame(width, height);
    window.swap_buffers();
  }
  return 0;
}

}  // namespace visual_integrator
