#include "display.hpp"
#include "mathgrapher.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

#define PI_SYMBOL "π"

namespace mg {

  class graphRenderer : public renderer {
    int counter;
    grapher* mainGrapher;
    color bg, ax, gr, gd, pisc, segr, transp;
    font def, intr;
  public:
    graphRenderer(grapher* g) {
      mainGrapher = g;
      counter = 0;
      bg = {255, 255, 255, 255};
      ax = {0, 0, 0, 255};
      gr = {255, 0, 0, 255};
      segr = {55, 200, 0, 255};
      gd = {220, 230, 255, 255};
      transp = {0, 0, 0, 0};
      pisc = {0, 0, 200, 255};
      def = {.size = 10, .weight = 400, .family = "Consolas"};
      intr = {.size = 12, .weight = 400, .family = "Consolas"};
    }
    virtual void update(display* disp) {
      context* con = disp->getContext();
      int w = con->getWidth(), h = con->getHeight();

      double scale = mainGrapher->getGraphParamsTable()->scale;
      int axx = w/2 + mainGrapher->getGraphParamsTable()->basisX;
      int axy = h/2 + mainGrapher->getGraphParamsTable()->basisY;
      int grth = mainGrapher->getGraphParamsTable()->weight ? 2 : 1;

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
      double yxscl = pow(10, mainGrapher->getGraphParamsTable()->scprop);

      if(mainGrapher->getGraphParamsTable()->grid) {
        for (double i = axx; i < w; i += scw) {
          if(i < 0) continue;
          con->drawLine((int) i, 0, (int) i, h, {gd, 1});
        }
        for (double i = axx; i >= 0; i -= scw) {
          if(i > w) continue;
          con->drawLine((int) i, 0, (int) i, h, {gd, 1});
        }
        for (double i = axy; i < h; i += scw) {
          if(i < 0) continue;
          con->drawLine(0, (int) i, w, (int) i, {gd, 1});
        }
        for (double i = axy; i >= 0; i -= scw) {
          if(i > h) continue;
          con->drawLine(0,(int) i, w, (int) i, {gd, 1});
        }
      }

      con->drawLine(0, axy, w, axy, {ax, 2});
      con->drawLine(axx, 0, axx, h, {ax, 2});

      int incr = 2 * grth;

      for (int i = 0; i < w; i += incr) {
        int x1 = i;
        int x2 = i + incr;
        double xv1 = (x1 - axx) / scale;
        double xv2 = (x2 - axx) / scale;
        double yv1 = mainGrapher->calculateFunction(xv1);
        double yv2 = mainGrapher->calculateFunction(xv2);
        int y1 = (int)(axy - (yv1 * scale * yxscl));
        int y2 = (int)(axy - (yv2 * scale * yxscl));
        if(std::isnan(yv1) || std::isnan(yv2)) continue;
        // if(std::isinf(yv1)) {
        //   double yvmid = mainGrapher->calculateFunction((xv2 + xv1) / 2);
        //   if(std::isinf(yv2)) continue;
        //   if(yv1 > 0 && yvmid > yv2) yv1 = 4 * w / scale / yxscl;
        //   else if(yv1 < 0 && yvmid < yv2) yv1 = -4 * w / scale / yxscl;
        //   else continue;
        // } else if(std::isinf(yv2)) {
        //   double yvmid = std::abs(mainGrapher->calculateFunction((xv2 + xv1) / 2));
        //   if(yvmid > std::abs(yv1)) continue;//yv2 = -4 * w / scale / yxscl;
        // }
        if(std::fabs(yv1 * scale * yxscl) > 0x4fffffff) {
          if(std::fabs(yv2 * scale * yxscl) > 0x4fffffff) {
            continue;
          }
          if(yv1 > 0) {
            y1 = -4 * w;
          } else y1 = 4 * w;
        } else if(std::fabs(yv2 * scale * yxscl) > 0x4fffffff) {
          if(yv2 > 0) {
            y2 = -4 * w;
          } else y2 = 4 * w;
        }

        con->drawLine(x1, y1, x2, y2, {gr, grth});

      }

      double pvy2;

      if (mainGrapher->getGraphParamsTable()->second) {
        for (int i = 0; i < w; i += incr) {
          int x1 = i;
          int x2 = i + incr;
          double xv1 = (x1 - axx) / scale;
          double xv2 = (x2 - axx) / scale;
          double yv1 = mainGrapher->calculateSecondFunction(xv1);
          double yv2 = mainGrapher->calculateSecondFunction(xv2);
          int y1 = (int)(axy - (yv1 * scale * yxscl));
          int y2 = (int)(axy - (yv2 * scale * yxscl));
          if(std::isnan(yv1) || std::isnan(yv2)) continue;
          // if(std::isinf(yv1)) {
          //   double yvmid = mainGrapher->calculateFunction((xv2 + xv1) / 2);
          //   if(std::isinf(yv2)) continue;
          //   if(yv1 > 0 && yvmid > yv2) yv1 = 4 * w / scale / yxscl;
          //   else if(yv1 < 0 && yvmid < yv2) yv1 = -4 * w / scale / yxscl;
          //   else continue;
          // } else if(std::isinf(yv2)) {
          //   double yvmid = std::abs(mainGrapher->calculateFunction((xv2 + xv1) / 2));
          //   if(yvmid > std::abs(yv1)) continue;//yv2 = -4 * w / scale / yxscl;
          // }
          if(std::fabs(yv1 * scale * yxscl) > 0x4fffffff) {
            if(std::fabs(yv2 * scale * yxscl) > 0x4fffffff) {
              continue;
            }
            if(yv1 > 0) {
              y1 = -4 * w;
            } else y1 = 4 * w;
          } else if(std::fabs(yv2 * scale * yxscl) > 0x4fffffff) {
            if(yv2 > 0) {
              y2 = -4 * w;
            } else y2 = 4 * w;
          }
          con->drawLine(x1, y1, x2, y2, {segr, grth});

        }
      }

      if(mainGrapher->getGraphParamsTable()->scales && axy > -30 && axy < h + 30) {
        for (double i = axx; i < w; i+=scw) {
          if(i < 0) continue;
          con->drawLine((int) i, axy - 2, (int) i, axy + 2, {ax, 1});
          if(i != axx) con->drawText((int) i - 2, axy + 1, fastDTOS((i - axx) / scale), def, ax);
        }
        for (double i = axx; i >= 0; i-=scw) {
          if(i > w) continue;
          con->drawLine((int) i, axy - 2, (int) i, axy + 2, {ax, 1});
          if(i != axx) con->drawText((int) i - 5, axy + 1, fastDTOS((i - axx) / scale), def, ax);
        }
      }
      if(mainGrapher->getGraphParamsTable()->scales && axx > -50 && axx < w + 10) {
        for (double i = axy; i < h; i+=scw) {
          if(i < 0) continue;
          con->drawLine(axx - 2, (int) i, axx + 2, (int) i, {ax, 1});
          if(i != axy) con->drawText(axx + 2, (int)i - 7, fastDTOS(-(i - axy) / scale / yxscl), def, ax);
        }
        for (double i = axy; i >= 0; i-=scw) {
          if(i > h) continue;
          con->drawLine(axx - 2, (int) i, axx + 2, (int) i, {ax, 1});
          if(i != axy) con->drawText(axx + 2, (int)i - 7, fastDTOS(-(i - axy) / scale / yxscl), def, ax);
        }
      }

      if(mainGrapher->getGraphParamsTable()->piscales && axy > -30 && axy < h + 30) {
        for (double i = axx; i < w; i += (scale < scw ? scw : scale) * M_PI) {
          if(i < 0) continue;
          con->drawLine((int) i, axy - 3, (int) i, axy + 3, {pisc, 1});
          if(i != axx) con->drawText((int) i - 5, axy - 15, fastDTOS((i - axx) / scale / M_PI) + PI_SYMBOL, def, pisc);
        }
        for (double i = axx; i >= 0; i -= (scale < scw ? scw : scale) * M_PI) {
          if(i > w) continue;
          con->drawLine((int) i, axy - 3, (int) i, axy + 3, {pisc, 1});
          if(i != axx) con->drawText((int) i - 5, axy - 15, fastDTOS((i - axx) / scale / M_PI) + PI_SYMBOL, def, pisc);
        }
      }

      if(mainGrapher->getGraphParamsTable()->second) {
        int px = mainGrapher->getGraphParamsTable()->pointX;
        double pvx = (px - axx) / scale;
        pvy2 = mainGrapher->calculateSecondFunction(pvx);
        int py = (int)(axy - (pvy2 * scale * yxscl));
        con->drawCircle(px-4, py-4, px+3, py+3, bg, {segr, 1});
      }

      int px = mainGrapher->getGraphParamsTable()->pointX;
      int ppy = mainGrapher->getGraphParamsTable()->pointY;
      double pvx = (px - axx) / scale;
      double pvpy = (axy - ppy) / scale;
      double pvy = mainGrapher->calculateFunction(pvx);
      int py = (int)(axy - (pvy * scale * yxscl));
      con->drawCircle(px-4, py-4, px+3, py+3, bg, {gr, 1});
      con->drawLine(px-4, ppy, px+4, ppy, {ax, 2});
      con->drawLine(px, ppy-4, px, ppy+4, {ax, 2});
      if(mainGrapher->getGraphParamsTable()->overlay) {
        con->drawText(3, 0, "Press \'E\' to show graph controls", intr, ax);
        con->drawText(3, 12, "X = " + std::to_string(pvx), intr, ax);
        con->drawText(3, 24, "Y = " + std::to_string(pvpy) + " (pointer)", intr, ax);
        con->drawText(3, 36, "Y = " + std::to_string(pvy) + " (on graph)", intr, ax);
        if (mainGrapher->getGraphParamsTable()->second) con->drawText(3, 48, "Y = " + std::to_string(pvy2) + " (on second graph)", intr, ax);
        con->drawText(3, 60, "Scale by x: 1 unit = " + doubleToString(scale) + " px", intr, ax);
        con->drawText(3, 72, "Scale by y: 1 unit = " + doubleToString(scale * yxscl) + " px", intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 84, "FPS: " + std::to_string(getDebugStruct()->fps), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 96, "TPF: " + std::to_string(getDebugStruct()->tpf), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 108, "Width: " + std::to_string(w), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 120, "Height: " + std::to_string(h), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 132, "PointX: " + std::to_string(mainGrapher->getGraphParamsTable()->pointX), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 144, "PointY: " + std::to_string(mainGrapher->getGraphParamsTable()->pointY), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 156, "BasisX: " + std::to_string(axx), intr, ax);
        if (mainGrapher->getGraphParamsTable()->debug) con->drawText(3, 168, "BasisY: " + std::to_string(axy), intr, ax);
        con->drawRectangle(3, h - 13, 13, h - 3, gr, {transp, 0});
        con->drawText(16, h - 16, "y=" + mainGrapher->restoreFunctionString(), intr, ax);
        if (mainGrapher->getGraphParamsTable()->second) {
          con->drawRectangle(3, h - 26, 13, h - 16, segr, {transp, 0});
          con->drawText(16, h - 29, "y=" + mainGrapher->restoreSecondFunctionString(), intr, ax);
        }
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
