#include "visual_integrator/ui/lightmenu.h"
namespace visual_integrator {

void LightMenu::LightShow() {
  ImGui::Checkbox("start light game", &localLightsettings->lightgame);

  //--------------BACKGROUND--------------------
  ImGui::SeparatorText("BACKGROUND");
  //ImGui::Checkbox("background light", &localLightsettings->back_Light);
  ImGui::SliderFloat("intensity", &localLightsettings->back_intensity, 0.0f, 1.0f, "%.3f");

  //--------------DEFFUSE--------------------
  ImGui::SeparatorText("DEFFUSE");
  ImGui::Checkbox("deffuse", &localLightsettings->deffuse_Light);

  //--------------REFLECTION--------------------
  ImGui::SeparatorText("REFLECTION");
  ImGui::Checkbox("reflections", &localLightsettings->reflect_Light);
};

void LightMenu::SetLightData(LightSettings* light) { localLightsettings = light; }

}  // namespace visual_integrator