#include "display.hpp"
#include "mathgrapher.hpp"
#include <iostream>
#include <cmath>

#define PI_SYMBOL "π"

namespace mg {

  inline std::string fastDTOS(double v) {
    if(v == 0) return "0";
    std::string res = "";
    bool sign = false;
    if(v < 0) {
      sign = true;
      v = -v;
    }
    if(v > 0.99) {
      int integerPart = (int)round(v*10);
      char t = (char)('0' + (integerPart % 10));
      integerPart /= 10;
      res += std::to_string(integerPart);
      if(t > '0') {
        res += '.';
        res += t;
      }
    } else {
      int i;
      for (i = -2; i < 24 && v < 9.99; i++) {
        v *= 10;
      }
      v = round(v);
      if(i <= 5) {
        res = "0.";
        for (int j = 0; j < i; j++) {
          res += '0';
        }
        char t = (char)('0' + ((int)v % 10));
        v = (int)v / 10;
        res += (char)('0' + ((int)v % 10));
        if(t > '0') res += t;
      } else {
        char t = (char)('0' + ((int)v % 10));
        v = (int)v / 10;
        res += (char)('0' + ((int)v % 10));
        if(t > '0'){
          res += '.';
          res += t;
        }
        res += 'e';
        res += '-';
        res += std::to_string(i + 1);
      }
    }
    if(sign) return '-' + res;
    else return res;
  }

  inline std::string doubleToString(double v) {
    if(v == 0) return "0";
    std::string res = "";
    bool sign = false;
    if(v < 0) {
      sign = true;
      v = -v;
    }
    unsigned long long integerPart = (unsigned long long)v;
    v = v - integerPart;
    if(integerPart == 0) {
      if(v > 1e-9) {
        res = "0.";
        int ns = 0;
        int ip;
        for(int i = 0; i < 10; i++) {
          v *= 10;
          ip = (int)v;
          v = v - ip;
          if(ip) {
            for (int j = 0; j < ns; j++) {
              res += '0';
            }
            ns = 0;
            res += (char)('0' + ip);
          } else {
            ns++;
          }
        }
      } else {
        int i;
        for(i = 0; i < 24 && v < 1; i++) {
          v *= 10;
        }
        res += (char)('0' + (int)v);
        res += '.';
        for(int j = 0; j < 6; j++)  {
          v -= (int)v;
          v *= 10;
          res += (char)('0' + (int)v);
        }
        res += 'e';
        res += '-';
        res += std::to_string(i);
      }
    } else {
      while(integerPart) {
        res = (char)('0' + (integerPart % 10)) + res;
        integerPart /= 10;
      }
      if(v > 1e-9) {
        res += '.';
        int ns = 0;
        int ip;
        for(int i = 0; i < 10; i++) {
          v *= 10;
          ip = (int)v;
          v = v - ip;
          if(ip) {
            for (int j = 0; j < ns; j++) {
              res += '0';
            }
            ns = 0;
            res += (char)('0' + ip);
          } else {
            ns++;
          }
        }
      }
    }
    if(sign) return '-' + res;
    else return res;

  }

  class graphRenderer : public renderer {
    int counter;
    grapher* mainGrapher;
    color bg, ax, gr, gd, pisc, segr;
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

      if(mainGrapher->getGraphParamsTable()->scales) {
        for (double i = axx; i < w; i+=scw) {
          if(i < 0) continue;
          con->drawLine((int) i, axy - 2, (int) i, axy + 2, {ax, 1});
          if(i != axx) con->drawText((int) i - 2, axy + 10, fastDTOS((i - axx) / scale), def, ax);
        }
        for (double i = axx; i >= 0; i-=scw) {
          if(i > w) continue;
          con->drawLine((int) i, axy - 2, (int) i, axy + 2, {ax, 1});
          if(i != axx) con->drawText((int) i - 5, axy + 10, fastDTOS((i - axx) / scale), def, ax);
        }
        for (double i = axy; i < h; i+=scw) {
          if(i < 0) continue;
          con->drawLine(axx - 2, (int) i, axx + 2, (int) i, {ax, 1});
          if(i != axy) con->drawText(axx + 2, (int)i + 3, fastDTOS(-(i - axy) / scale / yxscl), def, ax);
        }
        for (double i = axy; i >= 0; i-=scw) {
          if(i > h) continue;
          con->drawLine(axx - 2, (int) i, axx + 2, (int) i, {ax, 1});
          if(i != axy) con->drawText(axx + 2, (int)i + 3, fastDTOS(-(i - axy) / scale / yxscl), def, ax);
        }
      }

      if(mainGrapher->getGraphParamsTable()->piscales) {
        for (double i = axx; i < w; i += (scale < scw ? scw : scale) * M_PI) {
          if(i < 0) continue;
          con->drawLine((int) i, axy - 3, (int) i, axy + 3, {pisc, 1});
          if(i != axx) con->drawText((int) i - 5, axy - 5, fastDTOS((i - axx) / scale / M_PI) + PI_SYMBOL, def, pisc);
        }
        for (double i = axx; i >= 0; i -= (scale < scw ? scw : scale) * M_PI) {
          if(i > w) continue;
          con->drawLine((int) i, axy - 3, (int) i, axy + 3, {pisc, 1});
          if(i != axx) con->drawText((int) i - 5, axy - 5, fastDTOS((i - axx) / scale / M_PI) + PI_SYMBOL, def, pisc);
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
        int y1 = (int)(axy - (yv1 * scale * yxscl));
        int y2 = (int)(axy - (yv2 * scale * yxscl));
        con->drawLine(x1, y1, x2, y2, {gr, 1});
      }

      double pvy2;

      if (mainGrapher->getGraphParamsTable()->second) {
        for (int i = 0; i < w; i+=2) {
          int x1 = i;
          int x2 = i + 2;
          double xv1 = (x1 - axx) / scale;
          double xv2 = (x2 - axx) / scale;
          double yv1 = mainGrapher->calculateSecondFunction(xv1);
          double yv2 = mainGrapher->calculateSecondFunction(xv2);
          int y1 = (int)(axy - (yv1 * scale * yxscl));
          int y2 = (int)(axy - (yv2 * scale * yxscl));
          con->drawLine(x1, y1, x2, y2, {segr, 1});
        }
        int px = mainGrapher->getGraphParamsTable()->pointX;
        double pvx = (px - axx) / scale;
        pvy2 = mainGrapher->calculateSecondFunction(pvx);
        int py = (int)(axy - (pvy2 * scale));
        con->drawCircle(px-4, py-4, px+3, py+3, bg, {segr, 1});
      }

      int px = mainGrapher->getGraphParamsTable()->pointX;
      int ppy = mainGrapher->getGraphParamsTable()->pointY;
      double pvx = (px - axx) / scale;
      double pvpy = (axy - ppy) / scale;
      double pvy = mainGrapher->calculateFunction(pvx);
      int py = (int)(axy - (pvy * scale));
      con->drawCircle(px-4, py-4, px+3, py+3, bg, {gr, 1});
      con->drawLine(px-4, ppy, px+4, ppy, {ax, 2});
      con->drawLine(px, ppy-4, px, ppy+4, {ax, 2});
      con->drawText(3, 12, "Press \'E\' to show graph controls", intr, ax);
      con->drawText(3, 24, "X = " + std::to_string(pvx), intr, ax);
      con->drawText(3, 36, "Y = " + std::to_string(pvpy) + " (pointer)", intr, ax);
      con->drawText(3, 48, "Y = " + std::to_string(pvy) + " (on graph)", intr, ax);
      if (mainGrapher->getGraphParamsTable()->second) con->drawText(3, 60, "Y = " + std::to_string(pvy2) + " (on second graph)", intr, ax);
    }
    virtual bool check() {
      return mainGrapher->isSomethingChanged();
    }
  };

  renderer* initRenderer(grapher* g) {
    return new graphRenderer(g);
  }
}
