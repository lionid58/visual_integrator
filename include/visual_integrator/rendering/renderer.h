#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "visual_integrator/ui/LightSettings.h"

namespace visual_integrator {

class Renderer {
 public:
	// Requires a current OpenGL context that outlives this renderer.
	Renderer();
	~Renderer();

	void render_frame() const;
	void SetSize(int framebuffer_width, int framebuffer_height);
	void SetDefoultPos();
    // вот так мы связываем меню и рендер
	void SetLightData(LightSettings* light);
	void SetVideoData();

	void BuildShader();
	void BuildViewMatrixs();
    void RotateObject(float delta_x, float delta_y);
    void ZoomObject(float delta_z);
    void RenderLightGame() const;

  private:
    LightSettings* localLightsettings = nullptr;
	/*
	// VAO - ?
	// VBO - vertices buffer object
	// EBO - element buffer object
	*/
	unsigned int VAO, VBO, EBO, shaderProgram;
	// это для осей координат
	unsigned int axes_VAO, axes_VBO;
	int screenWidth, screenHeight, vertexCount = 0;
 	// матрица накопления вращения
	glm::mat4 rotMatrix = glm::mat4(1.0f);
	// MVP матрицы - это произведение матрц модели * взгляда * проекции
	glm::mat4 main_MVP, axes_MVP;
	// расстояние от камеры
	float distance = 2.0f;
	void InitAxes();
    void RenderAxes() const;
};

}  // namespace visual_integrator
