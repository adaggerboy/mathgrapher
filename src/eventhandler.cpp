#include "event.hpp"
#include "mathgrapher.hpp"

#include <cmath>

using namespace mg;

namespace mg {
  class graphEventHandler : public event {
    grapher* mainGrapher;
    bool dragmode;
    int beginX, beginY, beginPX, beginPY;

  public:
    graphEventHandler(grapher*);
    virtual void handle(event);
  };


  graphEventHandler::graphEventHandler(grapher* g) {
    mainGrapher = g;
    dragmode = false;
  }

  void graphEventHandler::handle(event e) {
    switch (e.type) {
      case wheelRotated:
        mainGrapher->getGraphParamsTable()->pxscale += e.dy;
        mainGrapher->getGraphParamsTable()->scale = 10 * exp(mainGrapher->getGraphParamsTable()->pxscale/100.);
        mainGrapher->change();
      break;
      case mousePressed:
        dragmode = true;
        beginX = e.x;
        beginY = e.y;
        beginPX = mainGrapher->getGraphParamsTable()->basisX;
        beginPY = mainGrapher->getGraphParamsTable()->basisY;
      break;
      case mouseReleased:
        dragmode = false;
      break;
      case mouseMoved:
        if(dragmode) {
          mainGrapher->getGraphParamsTable()->basisX = beginPX + e.x - beginX;
          mainGrapher->getGraphParamsTable()->basisY = beginPY + e.y - beginY;
          mainGrapher->getGraphParamsTable()->pointX = e.x;
          mainGrapher->getGraphParamsTable()->pointY = e.y;
          mainGrapher->change();

        } else {
          mainGrapher->getGraphParamsTable()->pointX = e.x;
          mainGrapher->getGraphParamsTable()->pointY = e.y;
          mainGrapher->change();

        }
      break;

    }
  }

  eventHandler* initEventHandler(grapher* g) {
    return (eventHandler*)new graphEventHandler(g);
  }

}
