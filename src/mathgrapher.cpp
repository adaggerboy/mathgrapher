#include "event.hpp"
#include "display.hpp"


namespace mg {
  display* initQt(int, char**, renderer*);
  renderer* initRenderer();
}

using namespace mg;


int main(int argc, char *argv[]) {

  display* d = initQt(argc, argv, initRenderer());
  color s;
  s.red = 255;
  s.green = 255;
  s.blue = 255;
  s.alpha = 255;
  d->start();
  d->getContext()->fill(s);
  // d->getContext()->update();

  return 0;
}
