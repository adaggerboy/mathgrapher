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
    params.overlay = true;
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

void error(std::string mesg) {
  std::cerr << mesg << '\n';
  exit(1);
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

  int f = 0;

  for (int i = 1; i < argc; i++) {
    if(argv[i][0] == 'y' && argv[i][1] == '=') {
      try {
        functionalExpression* exp = generate(string(argv[i] + 2));
        if(f == 0) {
          graph->setFunction(exp);
          f++;
        } else if(f == 1) {
          graph->setSecondFunction(exp);
          graph->getGraphParamsTable()->second = true;
          f++;
        } else error("More than two functions are not supported yet");
      } catch (syntaxError e) {
        error(string("Invalid function:") + e.what());
      }
    } else {
      string var = "";
      int j;
      for (j = 0; argv[i][j] && argv[i][j] != '='; j++) {
        var += argv[i][j];
      }
      if(argv[i][j] == '=') {
        j++;
        try {
          globalVarTable.setValue(var, stod(string(argv[i] + j)));
        } catch (exception e) {
          error(string("Invalid param:") + argv[i]);
        }
      } else error(string("Invalid param:") + argv[i]);
    }
  }

  QApplication* qapp = new QApplication(argc, argv);
  display* qt = initQt(qapp, rend, ev, con);
  qapp->exec();

  return 0;
}
