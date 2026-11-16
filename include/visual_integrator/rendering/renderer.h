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

	unsigned int BuildShader(const char* vertexPath, const char* fragmentPath);
	void BuildViewMatrixs();
    void RotateObject(float delta_x, float delta_y);
    void ZoomObject(float delta_z);
    void RenderLightGame() const;

	// Управление свободной камерой для режима Light Game
    void MoveCamera(float forward, float right, float up, float dt);
    void RotateCamera(float deltaX, float deltaY);

  private:
    LightSettings* localLightsettings = nullptr;
	/*
	// VAO - ?
	// VBO - vertices buffer object
	// EBO - element buffer object
	*/
    unsigned int VAO, VBO, EBO, basicshaderProgram, lightshaderProgram;
	// это для осей координат и источника света
    unsigned int axes_VAO, axes_VBO, lightSourse_VAO, lightSourse_VBO;

	int screenWidth, screenHeight, vertexCount = 0;
 	// матрица накопления вращения
	glm::mat4 rotMatrix = glm::mat4(1.0f);
	// MVP матрицы - это произведение матрц модели * взгляда * проекции
	glm::mat4 main_MVP, light_main_MVP, axes_MVP, lightSourse_MVP;
	// mainModel
    glm::mat4 mainModel;
	// расстояние от камеры
	float distance = 6.0f;
	void InitAxes();
    void RenderAxes() const;

	void InitLightSource();
    void RenderLightSource() const;

	// Параметры свободной камеры
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;  // взгляд вдоль -Z
    float pitch = 0.0f;
};

}  // namespace visual_integrator
