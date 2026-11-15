#include "visual_integrator/mathcore/parser.h"
namespace visual_integrator {

void ParserFunc::ParseFunc(char* strFunc) { parserF.SetExpr(strFunc); }

ParserFunc::ParserFunc() {
  parserF.DefineVar("x", &var_x);
  parserF.DefineVar("y", &var_y);
  parserF.DefineVar("z", &var_z);
}

double ParserFunc::Evaluate(glm::dvec3 vec) {
  var_x = vec.x;
  var_y = vec.y;
  var_z = vec.z;
  return parserF.Eval();
}

}  // namespace visual_integrator
