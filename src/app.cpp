#include "visual_integrator/app.h"

namespace visual_integrator {

int run() 
{
    bool frameChange = true;
    bool prev_lightgame = false;
    LightSettings globalLightsettings;

	Window window;
	Renderer renderer;
    renderer.SetLightData(&globalLightsettings);
    Menu menu;
    menu.lightmenu.SetLightData(&globalLightsettings);
	ParserFunc parser;

	//glViewport(0, 0, framebuffer_width, framebuffer_height);
    renderer.SetVideoData();
	while (!window.should_close()) 
	{
		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

        // Если курсор в окне И зажата левая кнопка мыши
        // да и по идее матрицы меняються только при движении мыши,
        // так что перестраивать матрицы мы тоже будет здесь
        if (!io.WantCaptureMouse)
        {
            if (globalLightsettings.lightgame)
            {
                //========== УПРАВЛЕНИЕ В РЕЖИМЕ СВЕТА ==========
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) 
                {
                    renderer.RotateCamera(io.MouseDelta.x, io.MouseDelta.y);
                    frameChange = true;
                }

                // Перемещение WASD + Space (вверх) + LeftShift (вниз)
                float forward = 0.0f, right = 0.0f, up = 0.0f;
                if (ImGui::IsKeyDown(ImGuiKey_W)) forward += 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_S)) forward -= 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_D)) right += 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_A)) right -= 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_Space)) up += 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) up -= 1.0f;
                if (ImGui::IsKeyDown(ImGuiKey_Escape)) globalLightsettings.lightgame = false;

                if (forward != 0.0f || right != 0.0f || up != 0.0f) 
                {
                    renderer.MoveCamera(forward, right, up, io.DeltaTime);
                    frameChange = true;
                }
            }
            else
            {
                //========== УПРАВЛЕНИЕ В РЕЖИМЕ ПРОСМОТРА ==========
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    renderer.RotateObject(io.MouseDelta.x, io.MouseDelta.y);
                    frameChange = true;
                }
                if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                  renderer.SetDefoultPos();
                  frameChange = true;
                }
                if (io.MouseWheel != 0)
                {
                    renderer.ZoomObject(io.MouseWheel);
                    frameChange = true;
                }
            }

        }

        if (globalLightsettings.lightgame != prev_lightgame)
        {
          frameChange = true;
          prev_lightgame = globalLightsettings.lightgame;
        }

        //window.HideMouse(globalLightsettings.lightgame);
        menu.Show();
        ImGui::Render();

        const auto [width, height] = window.framebuffer_size();
        renderer.SetSize(width, height);

        if (frameChange || menu.changed )
        {
            renderer.BuildViewMatrixs();
            frameChange = false;
        }

        renderer.render_frame();

		// этот ништяк нужен, чтобы рисовать UI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (menu.startCalc)
		{
          parser.ParseFunc(menu.strFunc);
          menu.startCalc = false;
		}

		window.swap_buffers();
		window.poll_events();
	}

	return 0;
}

}  // namespace visual_integrator
