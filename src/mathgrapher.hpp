#ifndef MG_MAIN
#define MG_MAIN

#include "mathexpression.hpp"

namespace mg {

  struct graphParams {
    double scale;
    int pxscale;
    int basisX, basisY;
    int pointX, pointY, scprop;

    bool scales, grid, piscales, second;
  };

  class grapher {
    functionalExpression* function;
    functionalExpression* secondFunction;
    bool changed;
    graphParams params;
  public:
    grapher();
    void setFunction(functionalExpression* fun);
    void setSecondFunction(functionalExpression* fun);
    std::string restoreFunctionString();
    std::string restoreSecondFunctionString();
    double calculateFunction(double);
    double calculateSecondFunction(double);
    bool isSomethingChanged();
    graphParams* getGraphParamsTable();
    void change();
  };
}

#endif /* end of include guard: MG_MAIN */
