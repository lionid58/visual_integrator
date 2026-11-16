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
  // Шейдеров несколько, так что собиаем их в конструкторе
  basicshaderProgram = BuildShader("shaders/shader.vert", "shaders/shader.frag");
  lightshaderProgram = BuildShader("shaders/lightshader.vert", "shaders/lightshader.frag");
  shadowShaderProgram = BuildShader("shaders/shadow.vert", "shaders/shadow.frag");

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

  InitAxes();
  InitLightSource();
  InitShadowMap();
}

void Renderer::InitAxes() {
  float axesVertices[] = {
        // Ось X
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // стрелочки
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
        0.8f, 0.2f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.8f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f,
        // буковка
        1.15f, -0.15f, 0.0f, 1.0f, 0.0f, 0.0f, 
        1.35f, 0.15f, 0.0f, 1.0f, 0.0f, 0.0f, 
        1.15f, 0.15f, 0.0f, 1.0f, 0.0f, 0.0f, 
        1.35f, -0.15f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Ось Y
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // стрелочки
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.2f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.2f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f,
        // буковка
        0.0f, 1.10f, 0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 1.25f, 0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 1.25f, 0.0f, 0.0f, 1.0f, 0.0f, 
        -0.1f, 1.40f, 0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 1.25f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.1f, 1.40f, 0.0f, 0.0f, 1.0f, 0.0f,

        // Ось Z
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // стрелочки
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.2f, 0.8f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, -0.2f, 0.8f, 0.0f, 0.0f, 1.0f,
        // буковки не будет
  };

  glGenVertexArrays(1, &axes_VAO);
  glBindVertexArray(axes_VAO);

  // glGenBuffers — generate buffer object names
  glGenBuffers(1, &axes_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
  // эти строки передают данные на видеокарту
  // это вершины в локальных координатах
  glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);

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
void Renderer::InitLightSource() 
{
  float y = 0, z = 0, x = 0;
float LSVertices[] = {
    // Лицевая грань (z = 0.1f)
    x - 0.1f, y - 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y - 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y + 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y + 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y + 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y - 0.1f,   z + 0.1f,  1.0f, 1.0f, 1.0f,

    // Задняя грань (z = -0.1f)
    x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x + 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,

    // Левая грань (x = -0.1f)
    x - 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y - 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
    x - 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,

    // Правая грань (x = 0.1f)
     x + 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,

    // Нижняя грань (y = -0.1f)
     x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y - 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x - 0.1f, y - 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x - 0.1f, y - 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,

    // Верхняя грань (y = 0.1f)
     x - 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x + 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x - 0.1f, y + 0.1f, z + 0.1f,  1.0f, 1.0f, 1.0f,
     x - 0.1f, y + 0.1f, z - 0.1f,  1.0f, 1.0f, 1.0f
  };

  glGenVertexArrays(1, &lightSourse_VAO);
  glBindVertexArray(lightSourse_VAO);

  // glGenBuffers — generate buffer object names
  glGenBuffers(1, &lightSourse_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, lightSourse_VBO);
  // эти строки передают данные на видеокарту
  // это вершины в локальных координатах
  glBufferData(GL_ARRAY_BUFFER, sizeof(LSVertices), LSVertices, GL_STATIC_DRAW);

  // далее мы обьясняем видеокарте что значат переданные файлы:
  // ---первые три числа это КООРДИНАТЫ В ПРОСТРАНСТВЧЕ
  // ---оставшиеся это цвет
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);}

void Renderer::render_frame() const {
  
  if (localLightsettings->lightgame) 
  {
    RenderShadowPass();
  }
  
  // тут мы назначаем окно 
  glViewport(0, 0, screenWidth, screenHeight);
  // я очень захотел сиреневое окно, оно крутое
  float intensity = 1.0;
  if (localLightsettings->lightgame)
      intensity = localLightsettings->back_intensity;
  glClearColor(intensity * 42.0f / 255.0, intensity * 42.0f / 255.0, intensity * 53.0f / 255, 1.0f);
  // и отчищаем цвета
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int uni_MVP = 0, uni_model = 0;
  if (localLightsettings->lightgame)
  {
    glUseProgram(lightshaderProgram);

    // эта строка ищет строку MVP в таблице шейдеров, чтобы туда отправить
    // матрицы, так как эта строка одинакова, то будем искать её один раз
    uni_MVP = glGetUniformLocation(lightshaderProgram, "MVP");
    uni_model = glGetUniformLocation(lightshaderProgram, "model");

  } 
  else
  {
    glUseProgram(basicshaderProgram);
    // эта строка ищет строку MVP в таблице шейдеров, чтобы туда отправить
    // матрицы, так как эта строка одинакова, то будем искать её один раз
    uni_MVP = glGetUniformLocation(basicshaderProgram, "MVP");
  }
  RenderLightGame();
  // рисуем основную штуку:
  // отправляем нужные матрицы свободные или орбитные
  // свободная - управление на WASD
  // орбитная - только вокруг обьекта
  
  if (localLightsettings->lightgame)
  {
    glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, glm::value_ptr(light_main_MVP));
    glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(mainModel));
  }
  else
    glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, glm::value_ptr(main_MVP));
  // рисуем
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);

  
  
  if (!localLightsettings->lightgame) {
    // рисуем оси:
    // отправляем нужные матрицы
    glUniformMatrix4fv(uni_MVP, 1, GL_FALSE, glm::value_ptr(axes_MVP));
    // рисуем
    RenderAxes();
  } 
  else
  {
    // Включаем базовый шейдер для отрисовки самих светильников
    glUseProgram(basicshaderProgram);
    int basic_MVP = glGetUniformLocation(basicshaderProgram, "MVP");

    // Ставим максимальную яркость, чтобы светильники всегда были ярко-белыми
    int basic_Ambient = glGetUniformLocation(basicshaderProgram, "AmbientLight");
    glUniform1f(basic_Ambient, 1.0f);

    // Рисуем маленькую лампочку, если включена
    if (localLightsettings->lamp) {
      glUniformMatrix4fv(basic_MVP, 1, GL_FALSE, glm::value_ptr(lightSourse_MVP));
      RenderLightSource();
    }

    // Рисуем далёкое огромное солнце, если включено
    if (localLightsettings->sunLight) {
      glUniformMatrix4fv(basic_MVP, 1, GL_FALSE, glm::value_ptr(sunSourse_MVP));
      RenderLightSource();
    }
  }
}

void Renderer::SetSize(int framebuffer_width, int framebuffer_height) {
  screenWidth = framebuffer_width;
  screenHeight = framebuffer_height;
}
void Renderer::SetDefoultPos() {
  rotMatrix = glm::mat4(1.0f);
  distance = 6.0f;
}
void Renderer::SetLightData(LightSettings* light) { localLightsettings = light; };
void Renderer::SetVideoData() {
  // этот массив позже станет изменяемым по нажатию кнопки "start"
  float vertices[] = {// positions         // colors
                      -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
                      -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
                      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f,  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                      0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f};

  unsigned int indices[] = {0, 1, 4, 1, 5, 4, 2, 7, 3, 2, 6, 7, 0, 6, 2, 0, 4, 6,
                            1, 3, 7, 1, 5, 7, 0, 2, 3, 0, 3, 1, 4, 7, 6, 4, 5, 7};

  vertexCount = 36;

  // эти строки передают данные на видеокарту
  // это вершины в локальных координатах
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // это индесы вершин, чтобы в массиве вершин были только уникальные
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // для красоты добавим оси координат (как у всех нормальных людей)
  // в правый верхний угол
}

unsigned int Renderer::BuildShader(const char* vertexPath, const char* fragmentPath) {
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
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
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
  unsigned int shaderProgram = glCreateProgram();
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
  return shaderProgram;
}

void Renderer::BuildViewMatrixs(){
    glm::mat4 mainProj = 
        glm::perspective(glm::radians(60.0f), (float)screenWidth / screenHeight, 0.1f, 100.0f);
    if (localLightsettings->lightgame)
    {
      //--------------MAIN MATRIX-----------------
      mainModel = rotMatrix;
      glm::mat4 mainView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
      light_main_MVP = mainProj * mainView * mainModel;

      // 0. Матрица теней СОЛНЦА (лучи параллельны -> ортографическая проекция)
      if (localLightsettings->sunLight) {
        float yawRad = glm::radians(localLightsettings->sun_yaw);
        float pitchRad = glm::radians(localLightsettings->sun_pitch);
        glm::vec3 sunDir = glm::normalize(
            glm::vec3(cos(pitchRad) * cos(yawRad), sin(pitchRad), cos(pitchRad) * sin(yawRad)));

        // --- ВИЗУАЛЬНОЕ СОЛНЦЕ ---
        // Ставим куб в 50 юнитах от камеры в сторону источника
        glm::vec3 visualSunPos = cameraPos + sunDir * 50.0f;
        glm::mat4 sModel = glm::translate(glm::mat4(1.0f), visualSunPos);
        sModel = glm::scale(sModel, glm::vec3(5.0f));  // Делаем куб в 5 раз больше
        sunSourse_MVP = mainProj * mainView * sModel;
        // -------------------------

        glm::mat4 lightProj = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 20.0f);
        glm::mat4 lightView =
            glm::lookAt(sunDir * 6.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix[0] = lightProj * lightView;
      }

      // 1. Матрица кубика и теней ЛАМПОЧКИ (перспективная проекция на центр сцены)
      if (localLightsettings->lamp) {
        glm::vec3 pos = localLightsettings->point_light_pos;
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), pos);
        lightSourse_MVP = mainProj * mainView * cubeModel;

        glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 20.0f);
        // Защита от коллинеарности с вектором Up, если лампа строго над/под (0, 0, 0)
        glm::vec3 upVec = (std::abs(pos.x) < 0.001f && std::abs(pos.z) < 0.001f)
                              ? glm::vec3(0.0f, 0.0f, 1.0f)
                              : glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 lightView = glm::lookAt(pos, glm::vec3(0.0f), upVec);
        lightSpaceMatrix[1] = lightProj * lightView;
      }

      // 2. Матрица теней ПРОЖЕКТОРА (совпадает с позицией и взглядом камеры)
      if (localLightsettings->spotlight) {
        float fov = glm::clamp(localLightsettings->spot_outer_cutoff * 2.0f, 10.0f, 120.0f);
        glm::mat4 lightProj = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 25.0f);
        glm::mat4 lightView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        lightSpaceMatrix[2] = lightProj * lightView;
      }
    }
    else
    {
        //--------------MAIN MATRIX-----------------
        glm::vec3 orbitCamPos = glm::vec3(0.0f, 0.0f, distance);
        mainModel = rotMatrix;
        glm::mat4 mainView = glm::lookAt(orbitCamPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);

       
        main_MVP = mainProj * mainView * mainModel;
        //-----------------AXES MATRIX------------------
        glm::mat4 axesView = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
                                        glm::vec3(0.0f, 0.0f, -1.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 axesProj = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 10.0f);
        axes_MVP = axesProj * axesView * rotMatrix;
    }

}

void Renderer::RotateObject(float deltaX, float deltaY) {
  const float sensitivity = 0.005f;
  glm::mat4 delta_rot = glm::mat4(1.0f);
  delta_rot = glm::rotate(delta_rot, deltaX * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
  delta_rot = glm::rotate(delta_rot, deltaY * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
  rotMatrix = delta_rot * rotMatrix;
}

void Renderer::MoveCamera(float forward, float right, float up, float dt) {
  const float speed = 2.5f * dt;
  // Вектор "вправо" перпендикулярен взгляду и мировому верху
  glm::vec3 rightVec = glm::normalize(glm::cross(cameraFront, cameraUp));

  cameraPos += cameraFront * (forward * speed);
  cameraPos += rightVec * (right * speed);
  cameraPos += cameraUp * (up * speed);
}

void Renderer::RotateCamera(float deltaX, float deltaY) {
  const float sensitivity = 0.15f;
  yaw += deltaX * sensitivity;
  pitch -= deltaY * sensitivity;  // Инвертируем Y для привычного обзора

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

void Renderer::ZoomObject(float delta_z) {
  float zoomSpeed = 0.2f;
  distance += zoomSpeed * delta_z;

  if (distance < 0.5) distance = 0.5;
}

void Renderer::RenderAxes() const{
    // размер квадрата под оси
    const int axesSize = 120;
    // отступ слева
    const int paddingleft = 15;

    glViewport(screenWidth - axesSize - paddingleft, 
        screenHeight - axesSize - paddingleft,
        axesSize, axesSize);

    glClear(GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(axes_VAO);
    glLineWidth(3.0);
    glDrawArrays(GL_LINES, 0, 28);
}
void Renderer::RenderLightSource() const {
  glBindVertexArray(lightSourse_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::RenderLightGame() const {

    if (localLightsettings->lightgame)
    {
      const char* samplerNames[3] = {"shadowMapSun", "shadowMapLamp", "shadowMapSpot"};
      const char* matrixNames[3] = {"dirLight.lightSpaceMatrix", "pointLight.lightSpaceMatrix",
                                      "spotLight.lightSpaceMatrix"};

      for (int i = 0; i < 3; ++i) 
      {
          glActiveTexture(GL_TEXTURE0 + i);
          glBindTexture(GL_TEXTURE_2D, depthMap[i]);
          glUniform1i(glGetUniformLocation(lightshaderProgram, samplerNames[i]), i);
          glUniformMatrix4fv(glGetUniformLocation(lightshaderProgram, matrixNames[i]), 1, GL_FALSE,
                             glm::value_ptr(lightSpaceMatrix[i]));
      }
      glActiveTexture(GL_TEXTURE0);

      int uni_AmbientLight = glGetUniformLocation(lightshaderProgram, "AmbientLight");
      glUniform1f(uni_AmbientLight, localLightsettings->back_intensity);

      glUniform1i(glGetUniformLocation(lightshaderProgram, "useSun"),
                  localLightsettings->sunLight ? 1 : 0);
      glUniform1i(glGetUniformLocation(lightshaderProgram, "useLamp"),
                  localLightsettings->lamp ? 1 : 0);
      glUniform1i(glGetUniformLocation(lightshaderProgram, "useSpotlight"),
                  localLightsettings->spotlight ? 1 : 0);

      glUniform3fv(glGetUniformLocation(lightshaderProgram, "viewPos"), 1,
                   glm::value_ptr(cameraPos));
      glUniform1f(glGetUniformLocation(lightshaderProgram, "diffuseStrength"),
                  localLightsettings->diffuse_intensity);
      glUniform1f(glGetUniformLocation(lightshaderProgram, "specularStrength"),
                  localLightsettings->specular_intensity);
      glUniform1f(glGetUniformLocation(lightshaderProgram, "shininess"),
                  localLightsettings->shininess);

      // --- ДАННЫЕ ДЛЯ СОЛНЦА ---
      if (localLightsettings->sunLight) {
        float yawRad = glm::radians(localLightsettings->sun_yaw);
        float pitchRad = glm::radians(localLightsettings->sun_pitch);
        glm::vec3 sunDir(cos(pitchRad) * cos(yawRad), sin(pitchRad), cos(pitchRad) * sin(yawRad));

        glUniform3fv(glGetUniformLocation(lightshaderProgram, "dirLight.direction"), 1,
                     glm::value_ptr(sunDir));
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "dirLight.color"), 1,
                     glm::value_ptr(localLightsettings->sun_color));
      }

      // --- ДАННЫЕ ДЛЯ ЛАМПОЧКИ (КУБИКА) ---
      if (localLightsettings->lamp) {
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "pointLight.position"), 1,
                     glm::value_ptr(localLightsettings->point_light_pos));
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "pointLight.color"), 1,
                     glm::value_ptr(localLightsettings->lamp_color));
      }

      // --- ДАННЫЕ ДЛЯ ПРОЖЕКТОРА (ФОНАРИКА) ---
      if (localLightsettings->spotlight) {
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "spotLight.position"), 1,
                     glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "spotLight.direction"), 1,
                     glm::value_ptr(cameraFront));
        glUniform3fv(glGetUniformLocation(lightshaderProgram, "spotLight.color"), 1,
                     glm::value_ptr(localLightsettings->spot_color));

        // Передаём косинусы углов (в шейдере сравнение идёт через скалярное произведение dot)
        glUniform1f(glGetUniformLocation(lightshaderProgram, "spotLight.cutOff"),
                    glm::cos(glm::radians(localLightsettings->spot_cutoff)));
        glUniform1f(glGetUniformLocation(lightshaderProgram, "spotLight.outerCutOff"),
                    glm::cos(glm::radians(localLightsettings->spot_outer_cutoff)));
      }
    }
    else
    {
      int uni_AmbientLight = glGetUniformLocation(basicshaderProgram, "AmbientLight");
      glUniform1f(uni_AmbientLight, 1.0f);
    }

}

void Renderer::InitShadowMap() {
  glGenFramebuffers(3, depthMapFBO);
  glGenTextures(3, depthMap);

  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

  for (int i = 0; i < 3; ++i) {
    glBindTexture(GL_TEXTURE_2D, depthMap[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
    // Цветовой буфер не нужен, пишем только глубину
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderShadowPass() const {
  glUseProgram(shadowShaderProgram);
  glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "model"), 1, GL_FALSE,
                     glm::value_ptr(mainModel));

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  bool activeLights[3] = {localLightsettings->sunLight, localLightsettings->lamp,
                          localLightsettings->spotlight};

  int uni_lightSpace = glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix");

  for (int i = 0; i < 3; ++i) {
    if (!activeLights[i]) continue;

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(uni_lightSpace, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[i]));
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glDeleteVertexArrays(1, &axes_VAO);
  glDeleteBuffers(1, &axes_VBO);

  glDeleteVertexArrays(1, &lightSourse_VAO);
  glDeleteBuffers(1, &lightSourse_VBO);

  glDeleteProgram(basicshaderProgram);
  glDeleteProgram(lightshaderProgram);

  glDeleteFramebuffers(3, depthMapFBO);
  glDeleteTextures(3, depthMap);
  glDeleteProgram(shadowShaderProgram);
}

}  // namespace visual_integrator
