#include "visual_integrator/app.h"

namespace visual_integrator {

int run() 
{
	Window window;
	Renderer renderer;
    Menu menu;

	while (!window.should_close()) 
	{
		window.poll_events();
       
		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		menu.Show();

		ImGui::Render();

		const auto [width, height] = window.framebuffer_size();
		renderer.render_frame(width, height);

		// этот ништяк нужен, чтобы рисовать UI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swap_buffers();
	}

	return 0;
}

}  // namespace visual_integrator
