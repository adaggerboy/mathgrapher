#ifndef MG_MAIN
#define MG_MAIN

#include "mathexpression.hpp"

namespace mg {

  struct graphParams {
    double scale;
    int pxscale;
    int basisX, basisY;
    int pointX, pointY;

    bool scales, grid;
  };

  class grapher {
    functionalExpression* function;
    bool changed;
    graphParams params;
  public:
    grapher();
    void setFunction(functionalExpression* fun);
    std::string restoreFunctionString();
    double calculateFunction(double);
    bool isSomethingChanged();
    graphParams* getGraphParamsTable();
    void change();
  };
}

#endif /* end of include guard: MG_MAIN */
