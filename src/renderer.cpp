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
      if(mainGrapher->getGraphParamsTable()->grid) return;
      double scale = mainGrapher->getGraphParamsTable()->scale;
      int axx = w/2 + mainGrapher->getGraphParamsTable()->basisX;
      int axy = h/2 + mainGrapher->getGraphParamsTable()->basisY;
      con->fill(bg);
      con->drawLine(0, axy, w, axy, {ax, 2});
      con->drawLine(axx, 0, axx, h, {ax, 2});
      for (int i = 0; i < w; i+=2) {
        int x1 = i;
        int x2 = i + 2;
        double xv1 = (x1 - axx) / scale;
        double xv2 = (x2 - axx) / scale;
        double yv1 = mainGrapher->calculateY(xv1);
        double yv2 = mainGrapher->calculateY(xv2);
        int y1 = (int)(axy - (yv1 * scale));
        int y2 = (int)(axy - (yv2 * scale));
        con->drawLine(x1, y1, x2, y2, {gr, 1});
      }
      int px = mainGrapher->getGraphParamsTable()->pointX;
      double pvx = (px - axx) / scale;
      double pvy = mainGrapher->calculateY(pvx);
      int py = (int)(axy - (pvy * scale));
      con->drawCircle(px-3, py-3, px+3, py+3, bg, {gr, 1});
    }
    virtual bool check() {
      return mainGrapher->isSomethingChanged();
    }
  };

  renderer* initRenderer(grapher* g) {
    return new graphRenderer(g);
  }
}
