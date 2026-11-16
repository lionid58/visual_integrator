#pragma once
#include <imgui.h>
#include <iostream>
#include "visual_integrator/ui/LightSettings.h"


namespace visual_integrator {

class LightMenu {

public:
  LightMenu() = default;
  ~LightMenu() = default;

  // bool для того чтобы следить за изменениями
  bool LightShow();
  // вот так мы связываем меню и рендер
  void SetLightData(LightSettings* light);

private:
  LightSettings* localLightsettings = nullptr;
};

}  // namespace visual_integrator