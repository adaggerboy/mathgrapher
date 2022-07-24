#ifndef MG_EVENT_HEADER
#define MG_EVENT_HEADER

#include "mathgrapher.hpp"

namespace mg {

  struct event {
    enum eventType {mouseMoved, mousePressed, mouseReleased, windowResized, wheelRotated, keyPressed, keyReleased} type;
    int x, y, dx, dy, button;
  };

  class eventHandler {
  public:
    virtual void handle(event) = 0;
  };

}



#endif /* end of include guard: MG_EVENT_HEADER */
