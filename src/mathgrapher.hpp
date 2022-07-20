#ifndef MG_MAIN
#define MG_MAIN

#include "mathexpression.hpp"

namespace mg {

  struct graphParams {
    double scale;
    int pxscale;
  };

  class grapher {
    functionalExpression* function;
    bool changed;
    graphParams params;
  public:
    grapher();
    void setFunction(functionalExpression* fun);
    double calculateY(double);
    bool isSomethingChanged();
    graphParams* getGraphParamsTable();
    void change();
  };
}

#endif /* end of include guard: MG_MAIN */
