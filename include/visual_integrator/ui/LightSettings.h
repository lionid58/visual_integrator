#pragma once
#include <glm/glm.hpp>

namespace visual_integrator {

// вся структура посвящена тому, 
// чтобы очень просто и быстро передавали данные из
// меню в рендер
struct LightSettings {
  // если выключена, то матрицы света единичные и не мешают
  // если включено, пойдёт игра со светом.
  bool lightgame = false;

  bool sunLight = false;   // бесконечно удалённый источник света
  bool lamp = false;	   // точечный источник света
  bool spotlight  = false; // направленный 

  //--------------BACKGROUND PARAMETRS--------------------
  // интенсивность фонового света
  float back_intensity = 0.140f;

 //--------------------------SUN--------------------------
  float sun_yaw = 45.0f;
  float sun_pitch = 45.0f;
  glm::vec3 sun_color = glm::vec3(1.0f, 1.0f, 1.0f);


  //---------------------------LAMP-----------------------
  // Координаты точечного света
  glm::vec3 point_light_pos = glm::vec3(2.0f, 2.0f, 2.0f);
  glm::vec3 lamp_color = glm::vec3(1.0f, 1.0f, 1.0f);

  //--------------SPOTLIGHT-------------------
  float spot_cutoff = 12.5f;        // Внутренний угол яркого конуса (в градусах)
  float spot_outer_cutoff = 17.5f;  // Внешний угол плавного затухания (в градусах)
  glm::vec3 spot_color = glm::vec3(1.0f, 1.0f, 1.0f);


  //--------------ОБЩИЕ СВОЙСТВА МАТЕРИАЛА----------------------------------
  float specular_intensity = 0.500f;  // Яркость бликов
  float shininess = 32.0f;            // Резкость блика (степень блеска)
  float diffuse_intensity = 0.800f;   
};

}  // namespace visual_integrator