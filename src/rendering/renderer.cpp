#include "visual_integrator/rendering/renderer.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "shaders.h"

namespace visual_integrator {
namespace {
unsigned int compile_shader(unsigned int type, const char* source) {
  const auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  int ok = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    int length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(std::max(length, 1), '\0');
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    glDeleteShader(shader);
    throw std::runtime_error("Shader compilation failed: " + log);
  }
  return shader;
}
void attributes() {
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}
}  // namespace
Renderer::Renderer() {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    throw std::runtime_error("Failed to load OpenGL");
  glEnable(GL_DEPTH_TEST);
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  attributes();
  const float axes[] = {
      0,    0,     0,     1, 0,   0, 1,     0,     0,     1, 0,   0,
      1,    0,     0,     1, 0,   0, .8f,   .12f,  0,     1, 0,   0,
      1,    0,     0,     1, 0,   0, .8f,   -.12f, 0,     1, 0,   0,
      1.1f, -.1f,  0,     1, 0,   0, 1.3f,  .1f,   0,     1, 0,   0,
      1.1f, .1f,   0,     1, 0,   0, 1.3f,  -.1f,  0,     1, 0,   0,
      0,    0,     0,     0, 1,   0, 0,     1,     0,     0, 1,   0,
      0,    1,     0,     0, 1,   0, .12f,  .8f,   0,     0, 1,   0,
      0,    1,     0,     0, 1,   0, -.12f, .8f,   0,     0, 1,   0,
      0,    1.1f,  0,     0, 1,   0, 0,     1.22f, 0,     0, 1,   0,
      0,    1.22f, 0,     0, 1,   0, -.1f,  1.32f, 0,     0, 1,   0,
      0,    1.22f, 0,     0, 1,   0, .1f,   1.32f, 0,     0, 1,   0,
      0,    0,     0,     0, .5f, 1, 0,     0,     1,     0, .5f, 1,
      0,    0,     1,     0, .5f, 1, .12f,  0,     .8f,   0, .5f, 1,
      0,    0,     1,     0, .5f, 1, -.12f, 0,     .8f,   0, .5f, 1,
      -.1f, .1f,   1.15f, 0, .5f, 1, .1f,   .1f,   1.15f, 0, .5f, 1,
      .1f,  .1f,   1.15f, 0, .5f, 1, -.1f,  -.1f,  1.15f, 0, .5f, 1,
      -.1f, -.1f,  1.15f, 0, .5f, 1, .1f,   -.1f,  1.15f, 0, .5f, 1};
  glGenVertexArrays(1, &axes_vao_);
  glGenBuffers(1, &axes_vbo_);
  glBindVertexArray(axes_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
  attributes();
}
void Renderer::BuildShader() {
  unsigned int vertex = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
  unsigned int fragment = 0, program = 0;
  try {
    fragment = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    int ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
      int length = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
      std::string log(std::max(length, 1), '\0');
      glGetProgramInfoLog(program, length, nullptr, log.data());
      throw std::runtime_error("Shader linking failed: " + log);
    }
  } catch (...) {
    glDeleteShader(vertex);
    if (fragment) glDeleteShader(fragment);
    if (program) glDeleteProgram(program);
    throw;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (program_) glDeleteProgram(program_);
  program_ = program;
}
void Renderer::SetVideoData(std::span<const TriangelMesh> slices,
                            glm::dvec3 min, glm::dvec3 max) {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  const auto center = (min + max) * 0.5;
  const auto extent = max - min;
  const double scale = 1.5 / std::max({extent.x, extent.y, extent.z});
  std::vector<int> slice_counts;
  for (const auto& mesh : slices) {
    const auto offset = vertices.size() / 6;
    for (const auto& v : mesh.vertices()) {
      glm::dvec3 p = (glm::dvec3(v) - center) * scale;
      vertices.insert(vertices.end(),
                      {float(p.x), float(p.y), float(p.z), .3f, .7f, .9f});
    }
    for (const auto& triangle : mesh.triangles())
      for (auto i : triangle.vertics) {
        if (i >= mesh.vertices().size() ||
            offset > std::numeric_limits<unsigned int>::max() ||
            i > std::numeric_limits<unsigned int>::max() - offset)
          throw std::runtime_error("Mesh index out of range");
        indices.push_back(static_cast<unsigned int>(offset + i));
      }
    if (indices.size() > size_t(std::numeric_limits<int>::max()))
      throw std::runtime_error("Mesh too large");
    slice_counts.push_back(static_cast<int>(indices.size()));
  }
  slice_index_counts_ = std::move(slice_counts);
  index_count_ = 0;
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_DYNAMIC_DRAW);
}
void Renderer::SetVisibleSlices(std::size_t count) {
  if (count > slice_index_counts_.size())
    throw std::out_of_range("Invalid slice count");
  index_count_ = count == 0 ? 0 : slice_index_counts_[count - 1];
}
void Renderer::render_frame() const {
  if (width_ <= 0 || height_ <= 0) return;
  glViewport(0, 0, width_, height_);
  glClearColor(42.f / 255, 42.f / 255, 53.f / 255, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(program_);
  glUniform1f(glGetUniformLocation(program_, "AmbientLight"),
              light_ && light_->lightgame ? light_->back_intensity : 1.f);
  const auto mvp = glGetUniformLocation(program_, "MVP");
  glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(main_mvp_));
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
  const int size = std::min({120, width_, height_});
  glViewport(width_ - size, height_ - size, size, size);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUniform1f(glGetUniformLocation(program_, "AmbientLight"), 1.f);
  glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(axes_mvp_));
  glBindVertexArray(axes_vao_);
  glDrawArrays(GL_LINES, 0, 34);
  glViewport(0, 0, width_, height_);
}
void Renderer::SetSize(int w, int h) {
  width_ = w;
  height_ = h;
}
void Renderer::SetLightData(LightSettings* light) { light_ = light; }
void Renderer::SetDefoultPos() {
  rotation_ = glm::mat4(1);
  distance_ = 2;
}
void Renderer::BuildViewMatrixs() {
  if (width_ <= 0 || height_ <= 0) return;
  auto view =
      glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  auto projection =
      glm::perspective(glm::radians(60.f), float(width_) / height_, .1f, 100.f);
  main_mvp_ = projection * view *
              glm::translate(glm::mat4(1), glm::vec3(0, 0, -distance_)) *
              rotation_;
  axes_mvp_ =
      glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, .1f, 10.f) *
      glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(0, 1, 0)) *
      rotation_;
}
void Renderer::RotateObject(float x, float y) {
  rotation_ = glm::rotate(glm::mat4(1), x * .005f, glm::vec3(0, 1, 0)) *
              glm::rotate(glm::mat4(1), y * .005f, glm::vec3(1, 0, 0)) *
              rotation_;
}
void Renderer::ZoomObject(float delta) {
  distance_ = std::clamp(distance_ - delta * .2f, .5f, 20.f);
}
Renderer::~Renderer() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteVertexArrays(1, &axes_vao_);
  glDeleteBuffers(1, &axes_vbo_);
  glDeleteProgram(program_);
}
}  // namespace visual_integrator
