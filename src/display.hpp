#ifndef MG_DISPLAY_HEADER
#define MG_DISPLAY_HEADER

#include <string>

#include "event.hpp"

namespace mg {

  struct color {
    uint8_t red, green, blue, alpha;
  };

  struct pen {
    color col;
    int thickness;
  };

  struct font {
    int size;
    int weight;
    std::string family;
  };

  struct perfDebugStruct {
    int fps;
    int tpf;
  };

  class display;

  class context {
  protected:
    int width, height;
  public:
    int getWidth() {
      return width;
    }
    int getHeight() {
      return height;
    }
    void setWidth(int w) {
      width = w;
    }
    void setHeight(int h) {
      height = h;
    }
    virtual void drawLine(int x1, int y1, int x2, int y2, pen p) = 0;
    virtual void fill(int x1, int x2, int y1, int y2, color c) = 0;
    virtual void fill(color c) = 0;
    virtual void drawRectangle(int x1, int y1, int x2, int y2, color c, pen p) = 0;
    virtual void drawCircle(int x1, int y1, int x2, int y2, color c, pen p) = 0;
    virtual void drawText(int x, int y, std::string text, font f, color c) = 0;
    virtual void setPixel(int x, int y, color c) = 0;
  };

  class renderer {
    perfDebugStruct dbg;
  public:
    perfDebugStruct* getDebugStruct() {
      return &dbg;
    }
    virtual void update(display*) = 0;
    virtual bool check() = 0;
  };

  class display {
  protected:
    renderer* rend;
    eventHandler* ev;
  public:
    display(renderer* r, eventHandler* e) : rend(r), ev(e) {}
    virtual void start() = 0;
    virtual context* getContext() = 0;
  };
}



#endif /* end of include guard: MG_DISPLAY_HEADER */
