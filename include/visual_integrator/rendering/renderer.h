#include <glm/glm.hpp>
#include <span>
#include <vector>

#include "visual_integrator/ui/LightSettings.h"
#include "visual_integrator/visualization/primitives.hh"

namespace visual_integrator {
class Renderer {
 public:
  Renderer();
  ~Renderer();
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  void render_frame() const;
  void SetSize(int width, int height);
  void SetDefoultPos();
  void SetLightData(LightSettings* light);
  void SetVideoData(std::span<const TriangelMesh> slices, glm::dvec3 min,
                    glm::dvec3 max);
  void SetVisibleSlices(std::size_t count);
  void BuildShader();
  void BuildViewMatrixs();
  void RotateObject(float x, float y);
  void ZoomObject(float delta);

 private:
  LightSettings* light_ = nullptr;
  unsigned int vao_ = 0, vbo_ = 0, ebo_ = 0, program_ = 0;
  unsigned int axes_vao_ = 0, axes_vbo_ = 0;
  int width_ = 1, height_ = 1, index_count_ = 0;
  std::vector<int> slice_index_counts_;
  glm::mat4 rotation_{1.0f}, main_mvp_{1.0f}, axes_mvp_{1.0f};
  float distance_ = 2.0f;
};
}  // namespace visual_integrator
