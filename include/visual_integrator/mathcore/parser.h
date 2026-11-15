#pragma once

#include <glm/vec3.hpp>

#include "muParser.h"

namespace visual_integrator {

class ParserFunc {
 public:
  mu::Parser parserF;
  double var_x = 0, var_y = 0, var_z = 0;

  void ParseFunc(char* strFunc);
  double Evaluate(glm::dvec3 vec);

  ParserFunc();
  ~ParserFunc() = default;

  // защита от копирования, чтобы адреса var_x, ... не полетели
  // ParserFunc(const ParserFunc&) = delete;
  // ParserFunc& operator=(const ParserFunc&) = delete;
};

}  // namespace visual_integrator