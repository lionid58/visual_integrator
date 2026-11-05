#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "visual_integrator/ui/lightmenu.h"
#include "visual_integrator/ui/LightSettings.h"

namespace visual_integrator{

class Menu{
public:
  float xRange[2] = {-1.0f, 1.0f};
  float yRange[2] = {-1.0f, 1.0f};
  float zRange[2] = {-1.0f, 1.0f};
  char strFunc[128] = "";

  bool startCalc = false;
  bool openDemoWind = false;
  bool openLightWind = false;

  Menu() = default;
  ~Menu() = default;
  void Show();

  LightMenu lightmenu;

 private:
  static void HelpMarker(const char* desc);
};

}  // namespace visual_integrator

