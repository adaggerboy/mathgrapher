#ifndef MG_MAIN
#define MG_MAIN

#include "mathexpression.hpp"

namespace mg {
  class grapher {
    functionalExpression* function;
    bool changed;
  public:
    grapher();
    void setFunction(functionalExpression* fun);
    double calculateY(double);
    bool isSomethingChanged();
  };
}

#endif /* end of include guard: MG_MAIN */
