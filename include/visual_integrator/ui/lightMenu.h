#pragma once
#include <imgui.h>
#include "visual_integrator/ui/LightSettings.h"


namespace visual_integrator {

class LightMenu {

public:
  LightMenu() = default;
  ~LightMenu() = default;
  void LightShow();
  // вот так мы связываем меню и рендер
  void SetLightData(LightSettings* light);

private:
  LightSettings* localLightsettings = nullptr;
};

}  // namespace visual_integrator