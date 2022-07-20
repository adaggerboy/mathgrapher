#include "event.hpp"
#include "mathgrapher.hpp"

#include <cmath>

using namespace mg;

eventHandler::eventHandler(grapher* g) {
  mainGrapher = g;
}

void eventHandler::handle(event e) {
  switch (e.type) {
    case wheelRotated:
      mainGrapher->getGraphParamsTable()->pxscale += e.dy;
      mainGrapher->getGraphParamsTable()->scale = 10 * exp(mainGrapher->getGraphParamsTable()->pxscale/100.);
      mainGrapher->change();
    break;

  }
}
