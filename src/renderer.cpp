#include "display.hpp"
#include "mathgrapher.hpp"
#include <iostream>

namespace mg {
  class graphRenderer : public renderer {
    int counter;
    grapher* mainGrapher;
    color bg, ax, gr;
  public:
    graphRenderer(grapher* g) {
      mainGrapher = g;
      counter = 0;
      bg = {255, 255, 255, 255};
      ax = {0, 0, 0, 255};
      gr = {255, 0, 0, 255};
    }
    virtual void update(display* disp) {
      context* con = disp->getContext();
      int w = con->getWidth(), h = con->getHeight();
      con->fill(bg);
      con->drawLine(0, h/2, w, h/2, {ax, 2});
      con->drawLine(w/2, 0, w/2, h, {ax, 2});
      for (int i = 0; i < w; i+=2) {
        int x1 = i;
        int x2 = i + 2;
        double xv1 = (x1 - w/2) / 10.;
        double xv2 = (x2 - w/2) / 10.;
        double yv1 = mainGrapher->calculateY(xv1);
        double yv2 = mainGrapher->calculateY(xv2);
        int y1 = (int)(h/2 - (yv1 * 10));
        int y2 = (int)(h/2 - (yv2 * 10));
        con->drawLine(x1, y1, x2, y2, {gr, 1});
      }
    }
    virtual bool check() {
      return mainGrapher->isSomethingChanged();
    }
  };

  renderer* initRenderer(grapher* g) {
    return new graphRenderer(g);
  }
}
