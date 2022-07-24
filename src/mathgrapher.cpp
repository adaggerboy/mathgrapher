#include "event.hpp"
#include "display.hpp"
#include "mathexpression.hpp"
#include "mathgrapher.hpp"
#include "controls.hpp"

#include <iostream>
#include <QApplication>
#include <cmath>

namespace mg {
  grapher::grapher() {
    function = NULL;
    secondFunction = NULL;
    changed = true;
    params.scale = 10.0;
    params.pxscale = 0;
    params.basisX = 0;
    params.basisY = 0;
    params.grid = true;
    params.scales = true;
    params.piscales = false;
  }
  void grapher::setFunction(functionalExpression* fun) {
    if(function) {
      function->destroy();
      delete function;
    }
    function = fun;
    changed = true;
  }
  double grapher::calculateFunction(double x) {
    if(!function) return 0;
    globalVarTable.setValue('x', x);
    return function->calculate();
  }
  bool grapher::isSomethingChanged() {
    bool res = changed;
    changed = false;
    return res;
  }

  graphParams* grapher::getGraphParamsTable() {
    return &params;
  }

  std::string grapher::restoreFunctionString() {
    if(!function) return "";
    return function->restoreExpression();
  }

  std::string grapher::restoreSecondFunctionString() {
    if(!secondFunction) return "";
    return secondFunction->restoreExpression();
  }


  void grapher::change() {
    changed = true;
  }

  void grapher::setSecondFunction(functionalExpression* fun) {
    if(secondFunction) {
      secondFunction->destroy();
      delete secondFunction;
    }
    secondFunction = fun;
    changed = true;
  }

  double grapher::calculateSecondFunction(double x) {
    if(!secondFunction) return 0;
    globalVarTable.setValue('x', x);
    return secondFunction->calculate();
  }

  display* initQt(QApplication*, renderer*, eventHandler*, controls*);
  renderer* initRenderer(grapher*);
  eventHandler* initEventHandler(grapher*);
  controls* initControls(grapher*);
}

int main(int argc, char *argv[]) {

  using namespace mg;
  using namespace std;

  grapher* graph = new grapher;
  renderer* rend = initRenderer(graph);
  eventHandler* ev = initEventHandler(graph);
  controls* con = initControls(graph);

  globalVarTable.setValue("pi", M_PI);
  globalVarTable.setValue("phi", (1 + sqrt(5))/2);
  globalVarTable.setValue("e", M_E);

  try {
    functionalExpression* exp = generate(argv[1]);
    globalVarTable.setValue('x', 2);
    graph->setFunction(exp);

  } catch (syntaxError e) {
    cout << e.what() << endl;
  }

  QApplication* qapp = new QApplication(argc, argv);
  display* qt = initQt(qapp, rend, ev, con);
  qapp->exec();

  return 0;
}
