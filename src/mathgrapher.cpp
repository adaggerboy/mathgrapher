#include "event.hpp"
#include "display.hpp"
#include "mathexpression.hpp"
#include "mathgrapher.hpp"

#include <iostream>

namespace mg {
  grapher::grapher() {
    function = NULL;
    changed = true;
  }
  void grapher::setFunction(functionalExpression* fun) {
    function = fun;
    changed = true;
  }
  double grapher::calculateY(double x) {
    if(!function) return 0;
    globalVarTable.setValue('x', x);
    return function->calculate();
  }
  bool grapher::isSomethingChanged() {
    bool res = changed;
    changed = false;
    return res;
  }

  display* initQt(int, char**, renderer*);
  renderer* initRenderer(grapher*);
}

int main(int argc, char *argv[]) {

  using namespace mg;
  using namespace std;

  grapher* graph = new grapher;
  renderer* rend = initRenderer(graph);

  try {
    functionalExpression* exp = generate(argv[1]);
    globalVarTable.setValue('x', 2);
    graph->setFunction(exp);

  } catch (syntaxError e) {
    cout << e.what() << endl;
  }

  display* qt = initQt(argc, argv, rend);
  qt->start();

  return 0;
}
