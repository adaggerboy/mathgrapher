#include "display.hpp"

namespace mg {
  class graphRenderer : public renderer {
    int counter;
  public:
    graphRenderer() {
      counter = 0;
    }
    virtual void update(display* disp) {
      context* con = disp->getContext();
      color r;
      r.red = 255;
      r.green = 255;
      r.blue = 255;
      r.alpha = 255;
      con->fill(r);
      r.red = r.blue = r.green = 0;
      pen p;
      p.col = r;
      p.thickness = 2;

      font f;
      f.size = 12;
      f.weight = 900;
      f.family = "calibri";

      counter+=1;
    }
    virtual bool check() {
      return true;
    }
  };

  renderer* initRenderer() {
    return new graphRenderer();
  }
}
