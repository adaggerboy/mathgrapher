#include "event.hpp"
#include "display.hpp"
#include "mathexpression.hpp"
#include "mathgrapher.hpp"
#include "controls.hpp"

#include <iostream>
#include <QApplication>

namespace mg {
  grapher::grapher() {
    function = NULL;
    changed = true;
    params.scale = 10.0;
    params.pxscale = 0;
    params.basisX = 0;
    params.basisY = 0;
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

  graphParams* grapher::getGraphParamsTable() {
    return &params;
  }

  void grapher::change() {
    changed = true;
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
