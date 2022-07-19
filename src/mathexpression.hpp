#ifndef MG_MATH_EXPRESSION
#define MG_MATH_EXPRESSION

#include <map>
#include <string>

namespace mg {

  class varTable {
  protected:
    std::map<char, double> singleLetter;
    std::map<std::string, double> extended;
    double x, y, a;
  public:
    varTable();
    double getValue(std::string name);
    double getValue(char name);

    void setValue(std::string name, double value);
    void setValue(char name, double value);
  };

  extern varTable globalVarTable;

  class functionalExpression {
  public:
    virtual std::string restoreExpression() = 0;
    virtual void setArgument(functionalExpression* expression, int index) = 0;
    virtual double calculate() = 0;
    virtual void destroy() = 0;
  };

  const std::string syntaxErrors[] = {
    "Duplication of point in constant",
    "Unexpected symbols",
    "Unexpected syntax element",
    "Unexpected ending",
    "Expression with undefined priority",
    "Undefined function name",
    "Undefined double function name"
  };

  class syntaxError {
    int position;
    int code;
  public:
    syntaxError(int, int);
    std::string what();
  };
}

#endif /* end of include guard: MG_MATH_EXPRESSION */
