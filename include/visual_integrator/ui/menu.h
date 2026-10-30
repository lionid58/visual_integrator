#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace visual_integrator{


class Menu{
 public:
  float xRange[2] = {-1.0f, 1.0f};
  float yRange[2] = {-1.0f, 1.0f};
  float zRange[2] = {-1.0f, 1.0f};
  char strFunc[128] = "";

  bool startCalc = false;

  //ImGuiWindowFlags menu_flags = 0;
  //bool menu_open = true,
  bool openDemoWind = false;

  Menu() = default;
  ~Menu() = default;
  void Show();

  private:
  static void HelpMarker(const char* desc);
};

}  // namespace visual_integrator

