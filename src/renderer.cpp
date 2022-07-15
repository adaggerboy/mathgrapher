#include "display.hpp"

namespace mg {
  class graphRenderer : public renderer {
    int counter;
  public:
    graphRenderer() {
    }
    virtual void update(display* disp) {
      context* con = disp->getContext();

    }
    virtual bool check() {
      return true;
    }
  };

  renderer* initRenderer() {
    return new graphRenderer();
  }
}
