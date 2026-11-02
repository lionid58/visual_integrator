#include "visual_integrator/app.h"
#include "visual_integrator/mathcore/parser.h"

namespace visual_integrator {

int run() 
{
	Window window;
	Renderer renderer;
    Menu menu;
	ParserFunc parser;

	//glViewport(0, 0, framebuffer_width, framebuffer_height);
    renderer.SetVideoData();
    renderer.BuildShader();
	while (!window.should_close()) 
	{
		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        menu.Show();
        ImGui::Render();

        const auto [width, height] = window.framebuffer_size();
        renderer.SetSize(width, height);

        ImGuiIO& io = ImGui::GetIO();

        // Если курсор в окне И зажата левая кнопка мыши
        // да и по идее матрицы меняються только при движении мыши,
        // так что перестраивать матрицы мы тоже будет здесь
        if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
          renderer.RotateObject(io.MouseDelta.x, io.MouseDelta.y);
          renderer.BuildViewMatrixs();
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
