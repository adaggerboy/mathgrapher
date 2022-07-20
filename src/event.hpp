#ifndef MG_EVENT_HEADER
#define MG_EVENT_HEADER

#include "mathgrapher.hpp"

namespace mg {

  enum eventType {mouseMoved, mousePressed, mouseReleased, windowResized, wheelRotated, keyPressed, keyReleased};

  struct event {
    eventType type;
    int x, y, dx, dy, button;
  };

  class eventHandler {
    grapher* mainGrapher;
  public:
    eventHandler(grapher*);
    virtual void handle(event);
  };

}



#endif /* end of include guard: MG_EVENT_HEADER */
