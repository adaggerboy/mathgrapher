#include "event.hpp"
#include "mathgrapher.hpp"

#include <cmath>

using namespace mg;

namespace mg {
  class graphEventHandler : public event {
    grapher* mainGrapher;
    bool dragmode;
    bool ctrl;
    int beginX, beginY, beginPX, beginPY, w, h;

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
        {
          double oldscale = mainGrapher->getGraphParamsTable()->scale;
          mainGrapher->getGraphParamsTable()->pxscale += e.dy / 10;
          double newscale = 10 * exp(mainGrapher->getGraphParamsTable()->pxscale/100.);
          if(newscale > 500000000) {
            newscale = 500000000;
            mainGrapher->getGraphParamsTable()->pxscale = 100*log(newscale / 10) + 1;
          }
          if(newscale < 5e-7) {
            newscale = 5e-7;
            mainGrapher->getGraphParamsTable()->pxscale = 100*log(newscale / 10) - 1;
          }
          mainGrapher->getGraphParamsTable()->scale = newscale;
          mainGrapher->getGraphParamsTable()->basisX = (w/2 + mainGrapher->getGraphParamsTable()->basisX - mainGrapher->getGraphParamsTable()->pointX) * newscale / oldscale + mainGrapher->getGraphParamsTable()->pointX - w/2;
          mainGrapher->getGraphParamsTable()->basisY = (h/2 + mainGrapher->getGraphParamsTable()->basisY - mainGrapher->getGraphParamsTable()->pointY) * newscale / oldscale + mainGrapher->getGraphParamsTable()->pointY - h/2;
          mainGrapher->change();
        }

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
      case windowResized:
        w = e.x;
        h = e.y;
      break;

    }
  }

  eventHandler* initEventHandler(grapher* g) {
    return (eventHandler*)new graphEventHandler(g);
  }

}
