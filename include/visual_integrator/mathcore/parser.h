#pragma once
#include "iostream"
#include "muParser.h"

namespace visual_integrator {

class ParserFunc {

public:
  mu::Parser parserF;
 double var_x = 0, var_y = 0, var_z = 0;

  void ParseFunc(char* strFunc);
  double Evaluate(double var_x, double var_y, double var_z);

   ParserFunc();
  ~ParserFunc() = default;

  // защита от копирования, чтобы адреса var_x, ... не полетели
  //ParserFunc(const ParserFunc&) = delete;
  //ParserFunc& operator=(const ParserFunc&) = delete;
};

}  // namespace visual_integrator