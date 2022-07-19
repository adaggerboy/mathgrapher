#include "event.hpp"
#include "display.hpp"
#include "mathexpression.hpp"

#include <iostream>

namespace mg {
  display* initQt(int, char**, renderer*);
  renderer* initRenderer();
  functionalExpression* generate(std::string, int = 0);
}

using namespace mg;


int main(int argc, char *argv[]) {

  using namespace std;

  try {
    functionalExpression* exp = generate("2sin(alpha-beta^2)+2*7(a+b)(a-b)");
    globalVarTable.setValue('x', 1);
    std::cout << exp->restoreExpression() << '\n';

  } catch (syntaxError e) {
    cout << e.what() << endl;
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
