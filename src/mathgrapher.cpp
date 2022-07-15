#include "event.hpp"
#include "display.hpp"
#include "mathexpression.hpp"

#include <iostream>

namespace mg {
  display* initQt(int, char**, renderer*);
  renderer* initRenderer();
  functionalExpression* generate();
}

using namespace mg;


int main(int argc, char *argv[]) {

  using namespace std;

  functionalExpression* exp = generate();

  for (int i = 0; i < 10; i++) {
    globalVarTable.setValue('x', i / 2.);
    cout << exp->calculate() << endl;
  }


  // display* d = initQt(argc, argv, initRenderer());
  // color s;
  // s.red = 255;
  // s.green = 255;
  // s.blue = 255;
  // s.alpha = 255;
  // d->start();
  // d->getContext()->fill(s);
  // d->getContext()->update();

  return 0;
}
