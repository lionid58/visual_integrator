#include "visual_integrator/ui/lightmenu.h"
namespace visual_integrator {

bool LightMenu::LightShow() {

  bool changed = false;
  ImGui::Checkbox("start light game", &localLightsettings->lightgame);
  if (!localLightsettings->lightgame) return changed;
  //--------------BACKGROUND--------------------
  ImGui::SeparatorText("BACKGROUND");
  //ImGui::Checkbox("background light", &localLightsettings->back_Light);
  ImGui::SliderFloat("intensity", &localLightsettings->back_intensity, 0.0f, 1.0f, "%.3f");

  //--------------LIGHT SOURCES (ВЫПАДАЮЩИЕ СПИСКИ)-------
  ImGui::SeparatorText("LIGHT SOURCES");

  if (ImGui::CollapsingHeader("Sun (Directional)")) {
    ImGui::Indent();
    changed |= ImGui::Checkbox("Enable Sun", &localLightsettings->sunLight);
    ImGui::ColorEdit3("Sun RGB", &localLightsettings->sun_color.x);
    changed |= ImGui::SliderFloat("Azimuth (yaw)", &localLightsettings->sun_yaw, -180.0f, 180.0f,
                                  "%.1f deg");
    changed |= ImGui::SliderFloat("Elevation (pitch)", &localLightsettings->sun_pitch, -89.0f,
                                  89.0f, "%.1f deg");
    ImGui::Unindent();
  }

  if (ImGui::CollapsingHeader("Point Cube (Lamp)")) {
    ImGui::Indent();
    changed |= ImGui::Checkbox("Enable Lamp", &localLightsettings->lamp);
    ImGui::ColorEdit3("Lamp RGB", &localLightsettings->lamp_color.x);
    changed |= ImGui::SliderFloat3("Cube Position", 
        &localLightsettings->point_light_pos.x, -3.0f, 3.0f, "%.1f");
    ImGui::Unindent();
  }

  if (ImGui::CollapsingHeader("Spotlight (Flashlight)")) {
    ImGui::Indent();
    changed |= ImGui::Checkbox("Enable Spotlight", &localLightsettings->spotlight);
    ImGui::ColorEdit3("Spot RGB", &localLightsettings->spot_color.x);
    ImGui::SliderFloat("Inner Angle", &localLightsettings->spot_cutoff, 1.0f, 45.0f, "%.1f deg");
    changed |= ImGui::SliderFloat("Outer Angle", &localLightsettings->spot_outer_cutoff, 1.0f,
                                  60.0f, "%.1f deg");

    // Защита: внешний угол конуса не должен быть меньше внутреннего
    if (localLightsettings->spot_outer_cutoff < localLightsettings->spot_cutoff) {
      localLightsettings->spot_outer_cutoff = localLightsettings->spot_cutoff + 0.5f;
    }
    ImGui::Unindent();
  }

//--------------SURFACE / REFLECTION--------------------
  ImGui::SeparatorText("SURFACE MATERIAL");
  ImGui::SliderFloat("diffuse intensity", &localLightsettings->diffuse_intensity, 0.0f, 2.0f,
                     "%.3f");
  ImGui::SliderFloat("specular intensity", &localLightsettings->specular_intensity, 0.0f, 2.0f,
                     "%.3f");
  ImGui::SliderFloat("shininess", &localLightsettings->shininess, 2.0f, 128.0f, "%.0f");

  return changed;
};

void LightMenu::SetLightData(LightSettings* light) { localLightsettings = light; }

}  // namespace visual_integrator