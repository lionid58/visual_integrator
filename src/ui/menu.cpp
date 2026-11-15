#include "visual_integrator/ui/menu.h"
namespace visual_integrator {

void Menu::Show() {
  if (openDemoWind) ImGui::ShowDemoWindow();

  // здесь мы узнаём параметры окна
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  // Set menu window on the loeft side
  // and set constant wight
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(ImVec2(300, viewport->WorkSize.y));
  ImGui::Begin("Menu");

  if (ImGui::BeginTabBar("MainTabBar")) {
    if (ImGui::BeginTabItem("Math & Grid")) {
      //--------------FUNCTION AND AREA-------------
      ImGui::SeparatorText("FUNCTION AND AREA");
      // field for entering a function:
      ImGui::Text("Function f(x, y, z):");
      ImGui::TextDisabled("Inside: f(x, y, z) < 0");
      // make text field use all width of menu
      ImGui::SetNextItemWidth(-1.0f);
      // ##_name that way we don't make label on the left side
      ImGui::InputText("##func_input", strFunc, IM_COUNTOF(strFunc));

      ImGui::Text("Ranges:");
      ImGui::InputFloat2("(x)", xRange);
      ImGui::InputFloat2("(y)", yRange);
      ImGui::InputFloat2("(z)", zRange);
      //--------------VOXEL GRID--------------------
      ImGui::SeparatorText("VOXEL GRID");
      ImGui::InputInt("Cells per axis", &cells);
      ImGui::TextDisabled("Allowed: 2 to 128");
      ImGui::Combo("Cut axis", &axis, "X\0Y\0Z\0");

      //--------------ANIMATION PARAMS--------------
      ImGui::SeparatorText("ANIMATION PARAMETERS");
      ImGui::InputFloat("Seconds per slice", &interval, 0.05f, 0.5f, "%.2f");

      //--------------DELETE LATER------------------
      ImGui::SeparatorText("DELETE LATER");
      ImGui::Checkbox("Demo window", &openDemoWind);

      //--------------BUTTON------------------
      if (ImGui::Button("start")) {
        startCalc = true;
      }
      if (calculating) ImGui::TextUnformatted("Calculating...");
      if (!status.empty()) ImGui::TextWrapped("%s", status.c_str());

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Light game")) {
      lightmenu.LightShow();

      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}

void Menu::HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

}  // namespace visual_integrator
