#include "display.hpp"
#include "mathgrapher.hpp"
#include <iostream>
#include <cmath>

namespace mg {
  class graphRenderer : public renderer {
    int counter;
    grapher* mainGrapher;
    color bg, ax, gr, gd;
    font def;
  public:
    graphRenderer(grapher* g) {
      mainGrapher = g;
      counter = 0;
      bg = {255, 255, 255, 255};
      ax = {0, 0, 0, 255};
      gr = {255, 0, 0, 255};
      gd = {200, 200, 255, 255};
    }
    virtual void update(display* disp) {
      context* con = disp->getContext();
      int w = con->getWidth(), h = con->getHeight();

      double scale = mainGrapher->getGraphParamsTable()->scale;
      int axx = w/2 + mainGrapher->getGraphParamsTable()->basisX;
      int axy = h/2 + mainGrapher->getGraphParamsTable()->basisY;

      con->fill(bg);

      double scl = scale;
      int axw = 0;
      while(scl >= 100) {
        scl /= 10;
        axw++;
      }
      while(scl < 10) {
        scl *= 10;
        axw--;
      }
      double scw = scale / pow(10, axw);

      if(mainGrapher->getGraphParamsTable()->grid) {


        for (double i = axx; i < w; i+=scw) {
          if(i < 0) continue;
          con->drawLine((int) i, 0, (int) i, h, {gd, 1});
        }
        for (double i = axx; i >= 0; i-=scw) {
          if(i > w) continue;
          con->drawLine((int) i, 0, (int) i, h, {gd, 1});
        }
        for (double i = axy; i < h; i+=scw) {
          if(i < 0) continue;
          con->drawLine(0, (int) i, w, (int) i, {gd, 1});
        }
        for (double i = axy; i >= 0; i-=scw) {
          if(i > h) continue;
          con->drawLine(0,(int) i, w, (int) i, {gd, 1});
        }
      }

      con->drawLine(0, axy, w, axy, {ax, 2});
      con->drawLine(axx, 0, axx, h, {ax, 2});

      for (int i = 0; i < w; i+=2) {
        int x1 = i;
        int x2 = i + 2;
        double xv1 = (x1 - axx) / scale;
        double xv2 = (x2 - axx) / scale;
        double yv1 = mainGrapher->calculateFunction(xv1);
        double yv2 = mainGrapher->calculateFunction(xv2);
        int y1 = (int)(axy - (yv1 * scale));
        int y2 = (int)(axy - (yv2 * scale));
        con->drawLine(x1, y1, x2, y2, {gr, 1});
      }
      int px = mainGrapher->getGraphParamsTable()->pointX;
      double pvx = (px - axx) / scale;
      double pvy = mainGrapher->calculateFunction(pvx);
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
