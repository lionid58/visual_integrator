#include "visual_integrator/app.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <optional>
#include <string>
#include <utility>

#include "visual_integrator/visualization/area_pipeline.h"

namespace visual_integrator {
namespace {
struct Outcome {
  std::optional<AreaResult> result;
  std::string error;
};
Outcome calculate(AreaRequest request) {
  try {
    return {build_area(request), {}};
  } catch (const mu::Parser::exception_type& e) {
    return {std::nullopt, e.GetMsg()};
  } catch (const std::exception& e) {
    return {std::nullopt, e.what()};
  } catch (...) {
    return {std::nullopt, "Unknown calculation error"};
  }
}
}  // namespace
int run() {
  LightSettings light;
  Window window;
  Renderer renderer;
  renderer.SetLightData(&light);
  renderer.BuildShader();
  Menu menu;
  menu.lightmenu.SetLightData(&light);
  std::future<Outcome> job;
  std::optional<AreaRequest> pending;
  std::optional<AreaResult> current;
  size_t slice = 0;
  auto last_slice = std::chrono::steady_clock::now();
  auto start = [&](AreaRequest request) {
    job = std::async(std::launch::async, calculate, std::move(request));
    menu.calculating = true;
  };
  while (!window.should_close()) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        renderer.RotateObject(io.MouseDelta.x, io.MouseDelta.y);
      if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) renderer.SetDefoultPos();
      if (io.MouseWheel != 0) renderer.ZoomObject(io.MouseWheel);
    }
    menu.Show();
    if (menu.startCalc) {
      menu.startCalc = false;
      AreaRequest request{menu.strFunc,
                          {menu.xRange[0], menu.yRange[0], menu.zRange[0]},
                          {menu.xRange[1], menu.yRange[1], menu.zRange[1]},
                          menu.cells,
                          static_cast<Axis>(menu.axis)};
      menu.status.clear();
      if (job.valid())
        pending = std::move(request);
      else
        start(std::move(request));
    }
    if (job.valid() &&
        job.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      auto outcome = job.get();
      if (pending) {
        auto request = std::move(*pending);
        pending.reset();
        start(std::move(request));
      } else {
        menu.calculating = false;
        if (!outcome.result)
          menu.status = outcome.error;
        else {
          current = std::move(outcome.result);
          slice = 0;
          last_slice = std::chrono::steady_clock::now();
          if (current->slices.empty()) {
            menu.status = "No slices in this area";
            renderer.SetVideoData(current->slices, current->min, current->max);
          } else {
            menu.status = "Slices: " + std::to_string(current->slices.size());
            renderer.SetVideoData(current->slices, current->min, current->max);
            renderer.SetVisibleSlices(1);
          }
        }
      }
    }
    const auto now = std::chrono::steady_clock::now();
    const double interval = std::isfinite(menu.interval)
                                ? std::max(0.05, double(menu.interval))
                                : 0.3;
    if (current && current->slices.size() > 1 &&
        std::chrono::duration<double>(now - last_slice).count() >= interval) {
      slice = (slice + 1) % current->slices.size();
      renderer.SetVisibleSlices(slice + 1);
      last_slice = now;
    }
    // Keep the axis names legible even when their 3D strokes face the camera.
    auto* labels = ImGui::GetForegroundDrawList();
    const float label_x = std::max(0.0f, io.DisplaySize.x - 116.0f);
    labels->AddText(ImVec2(label_x, 4), IM_COL32(255, 70, 70, 255), "X");
    labels->AddText(ImVec2(label_x + 20, 4), IM_COL32(70, 255, 70, 255), "Y");
    labels->AddText(ImVec2(label_x + 40, 4), IM_COL32(70, 150, 255, 255), "Z");
    ImGui::Render();
    const auto [width, height] = window.framebuffer_size();
    renderer.SetSize(width, height);
    renderer.BuildViewMatrixs();
    renderer.render_frame();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    window.swap_buffers();
    window.poll_events();
  }
  return 0;
}
}  // namespace visual_integrator
