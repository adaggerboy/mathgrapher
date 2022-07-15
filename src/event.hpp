#ifndef MG_EVENT_HEADER
#define MG_EVENT_HEADER

#include <cstddef>

namespace mg {

  enum eventType {mouseMoved, mousePressed, mouseReleased, windowResized, wheelRotated};

  struct event {
    eventType type;
    int x, y, dx, dy, button;
  };

  class eventHandler {
  public:
    void handle(event);
  };

}



#endif /* end of include guard: MG_EVENT_HEADER */
