#include "visual_integrator/mathcore/parser.h"
namespace visual_integrator {

void ParserFunc::ParseFunc(char* strFunc) { 
	try {
          parserF.SetExpr(strFunc);
	} 
	catch (mu::Parser::exception_type& e) {
          std::cout << e.GetMsg() << std::endl;
    }
}

ParserFunc::ParserFunc()
{
  parserF.DefineVar("x", &var_x);
  parserF.DefineVar("y", &var_y);
}

double ParserFunc::Evaluate(double x, double y) {
  var_x = x;
  var_y = y;
  try {return parserF.Eval();} 
  catch (mu::Parser::exception_type& e){ std::cout << e.GetMsg() << std::endl; }
  
}

}  // namespace visual_integrator