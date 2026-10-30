#include "visual_integrator/ui/menu.h"
namespace visual_integrator {
void Menu::Show() {

	if (openDemoWind) ImGui::ShowDemoWindow();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	// Set menu window on the loeft side
	// and set constant wight
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(300, viewport->WorkSize.y));
	ImGui::Begin("Menu");
	//ImGui::Begin("Menu", &menu_open, menu_flags);

	//--------------FUNCTION AND AREA-------------
	ImGui::SeparatorText("FUNCTION AND AREA");
	// field for entering a function:
    ImGui::Text("Function f(x, y, z):");
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
    ImGui::SetNextItemWidth(-1.0);
    ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), 
		ImVec2(0.0f, 0.0f), "In develop...");

	//--------------ANIMATION PARAMS--------------
    ImGui::SeparatorText("ANIMATION PARAMETERS");
    ImGui::SetNextItemWidth(-1.0);
    ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), 
        ImVec2(0.0f, 0.0f), "In develop...");

	//--------------DELETE LATER------------------
    ImGui::SeparatorText("DELETE LATER");
    ImGui::Checkbox("Demo window", &openDemoWind);

    //--------------BUTTON------------------
    if (ImGui::Button("start"))
    {
        startCalc = true;
    }

    ImGui::End();
}

void Menu::HelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip())
  {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

}  // namespace visual_integrator