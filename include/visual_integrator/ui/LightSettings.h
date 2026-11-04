#pragma once

namespace visual_integrator {

// вся структура посвящена тому, 
// чтобы очень просто и быстро передавали данные из
// меню в рендер
struct LightSettings {
  // если выключена, то матрицы света единичные и не мешают
  // если включено, пойдёт игра со светом.
  bool lightgame = false;

  //--------------BACKGROUND PARAMETRS--------------------
  // наличие (да, мы должны быть способны полность выключть свет)
  //bool back_Light = false;
  // интенсивность фонового света
  float back_intensity = 1.000f;


  bool deffuse_Light = false;
  bool reflect_Light = false;
};

}  // namespace visual_integrator