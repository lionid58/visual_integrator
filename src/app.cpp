#include "visual_integrator/app.h"

#include "visual_integrator/platform/window.h"
#include "visual_integrator/rendering/renderer.h"

namespace visual_integrator {

int run() {
	Window window;
	Renderer renderer;
	while (!window.should_close()) 
	{
		window.poll_events();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();

		const auto [width, height] = window.framebuffer_size();
		renderer.render_frame(width, height);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swap_buffers();
	}

	window.CleanUpGUI();
	return 0;
}

}  // namespace visual_integrator
