#include "visual_integrator/rendering/renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <stdexcept>

#include <iostream>
#include <fstream>
#include <sstream>

namespace visual_integrator {

Renderer::Renderer() {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    throw std::runtime_error("Failed to load OpenGL");
  }
  // эта строка отвечает за обработку глубины
  glEnable(GL_DEPTH_TEST);
  // обработка мыши находиться в window.cpp

  /* так как функции GlGen... именно создают новые буферы
  // их надо вынести в конструкто, а иначе при обновления массива vertex[]
  // мы каждый раз будем генерить новый буфер, не удаляя старый
  // в общем это нужно от утечек памяти
  // 
  // Да и обьяснения формата данных всегда одно и тоже,
  // отчего это тоже будет здесь.
  */

  // glGenVertexArrays — generate vertex array object names
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // glGenBuffers — generate buffer object names
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // далее мы обьясняем видеокарте что значат переданные файлы:
  // ---первые три числа это КООРДИНАТЫ В ПРОСТРАНСТВЧЕ
  // ---оставшиеся это цвет
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Renderer::render_frame() const {

  // тут мы назначаем окно 
  glViewport(0, 0, screenWidth, screenHeight);
  // я очень захотел сиреневое окно, оно крутое
  glClearColor(42.0f / 255.0, 42.0f / 255.0, 53.0f / 255, 1.0f);
  // и отчищаем цвета
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
}

void Renderer::SetSize(int framebuffer_width, int framebuffer_height) {
  screenWidth = framebuffer_width;
  screenHeight = framebuffer_height;
}

void Renderer::SetVideoData() {

    //этот массив позже станет изменяемым по нажатию кнопки "start"
    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f,
        -0.5f, -0.5f, 0.5f,  1.0f, 0.0f,  0.0f,
        -0.5f, 0.5f, -0.5f,  0.0f, 0.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f,  1.0f,  
        0.5f,  0.5f,  -0.5f, 1.0f, 0.0f,  0.0f,  
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f
    };

    unsigned int indices[] = {
        0, 1, 4, 1, 5, 4,
        2, 7, 3, 2, 6, 7,
        0, 6, 2, 0, 4, 6,
        1, 3, 7, 1, 5, 7,
        0, 2, 3, 0, 3, 1,
        4, 7, 6, 4, 5, 7
    };
    vertexCount = sizeof(indices);

    // эти строки передают данные на видеокарту
    // это вершины в локальных координатах
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // это индесы вершин, чтобы в массиве вершин были только уникальные
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // для красоты добавим оси координат (как у всех нормальных людей)
    // в правый верхний угол

}

void Renderer::BuildShader() {

  // подключения шейдеров к графическому конвейру:
  // read shaders
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    vShaderFile.open("shaders/shader.vert");
    fShaderFile.open("shaders/shader.frag");
    std::stringstream vShaderStream, fShaderStream;
    // read file’s buffer contents into streams
    // здесь мы просто специфически читаем файл
    // (read buffer) возвращает указатель на внутренний низкоуровневый буфер открытого файла
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();

  // create vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  int success;
  char infoLog[512];
  glShaderSource(vertexShader, 1, &vShaderCode, NULL);
  glCompileShader(vertexShader);
  // print compile errors if any
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  // create fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
  glCompileShader(fragmentShader);
  // print compile errors if any
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  // shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // print linking errors if any
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  // delete shaders; they’re linked into our program and no longer necessary
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(shaderProgram);

}

void Renderer::BuildViewMatrixs(){

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    model *= rotMatrix;

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 proj = 
        glm::perspective(glm::radians(50.0f), (float)screenWidth / screenHeight, 0.1f, 100.0f);

    int uni_model = glGetUniformLocation(shaderProgram, "model");
    int uni_view = glGetUniformLocation(shaderProgram, "view");
    int uni_proj = glGetUniformLocation(shaderProgram, "proj");

    glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uni_proj, 1, GL_FALSE, glm::value_ptr(proj));
}

void Renderer::RotateObject(float deltaX, float deltaY) {
  const float sensitivity = 0.005f;
  glm::mat4 delta_rot = glm::mat4(1.0f);
  delta_rot = glm::rotate(delta_rot, deltaX * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
  delta_rot = glm::rotate(delta_rot, deltaY * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
  rotMatrix = delta_rot * rotMatrix;
}

Renderer::~Renderer()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
}

}  // namespace visual_integrator
