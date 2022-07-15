#include "mathexpression.hpp"

#include <vector>

namespace mg {

  varTable globalVarTable;

  varTable::varTable() {
    x = y = a = 0;
  }
  double varTable::getValue(std::string name) {
    if(name.length() == 1) return getValue(name[0]);
    return extended[name];
  }
  double varTable::getValue(char name) {
    switch (name) {
      case 'x': return x;
      case 'y': return y;
      case 'a': return a;
      default: return singleLetter[name];
    }
  }

  void varTable::setValue(std::string name, double value) {
    if(name.length() == 1) setValue(name[0], value);
    else extended[name]= value;
  }
  void varTable::setValue(char name, double value) {
    switch (name) {
      case 'x':
        x  = value;
        return;
      case 'y':
        y = value;
        return;
      case 'a':
        a = value;
        return;
      default:
        singleLetter[name] = value;
        return;
    }
  }

  class constant : public functionalExpression {
    double val;
  public:
    constant(double c) : val(c){}
    std::string restoreExpression() {
      return std::to_string(val);
    }
    void setArgument(functionalExpression* expression, int index) {}
    double calculate() {
      return val;
    }
    void destroy() {}
  };

  class variable : public functionalExpression {
    char var;
  public:
    variable(char v) : var(v){}
    std::string restoreExpression() {
      return std::string(1, var);
    }
    void setArgument(functionalExpression* expression, int index) {}
    double calculate() {
      return globalVarTable.getValue(var);
    }
    void destroy() {}
  };

  class extendedVariable : public functionalExpression {
    std::string var;
  public:
    extendedVariable(std::string v) : var(v){}
    std::string restoreExpression() {
      return var;
    }
    void setArgument(functionalExpression* expression, int index) {}
    double calculate() {
      return globalVarTable.getValue(var);
    }
    void destroy() {}
  };

  class quotient : public functionalExpression {
    double quot;
    functionalExpression* arg;
  public:
    quotient(double q, functionalExpression* a) : quot(q), arg(a) {}
    std::string restoreExpression() {
      if(quot == 1) return arg->restoreExpression();
      return std::to_string(quot) + arg->restoreExpression();
    }
    void setArgument(functionalExpression* expression, int index = 0) {
      if(index == 0) arg = expression;
    }
    double calculate() {
      return quot * arg->calculate();
    }
    void destroy() {
      arg->destroy();
      delete arg;
    }
  };

  class summ : public functionalExpression {
    std::vector<functionalExpression*> args;
  public:
    summ() {}
    summ(functionalExpression* a, functionalExpression* b) {
      args.push_back(a);
      args.push_back(b);
    }
    std::string restoreExpression() {
      std::string res = "(";
      if(args.size() == 0) return res + ")";
      res += args[0]->restoreExpression();
      if(args.size() == 1) return res + ")";
      int end = args.size();
      for (int i = 1; i < end; i++) {
        std::string el = args[i]->restoreExpression();
        if(el[0] != '-') res += '+';
        res += el;
      }
      return res + ")";

    }
    void setArgument(functionalExpression* expression, int index = -1) {
      if(index >= 0) args[index] = expression;
      else args.push_back(expression);
    }
    double calculate() {
      double res = 0;
      int end = args.size();
      for (int i = 0; i < end; i++) {
        res += args[i]->calculate();
      }
      return res;
    }
    void destroy() {
      for (int i = 0; i < args.size(); i++) {
        args[i]->destroy();
        delete args[i];
      }
    }
  };

  class fractionCombination : public functionalExpression {
    std::vector<functionalExpression*> num;
    std::vector<functionalExpression*> denom;
  public:
    fractionCombination() {}
    fractionCombination(functionalExpression* a, functionalExpression* b) {
      num.push_back(a);
      denom.push_back(b);
    }
    std::string restoreExpression() {
      std::string res = "";
      int end1 = num.size();
      int end2 = denom.size();
      if(end1 == 0 && end2 == 0) return res;
      if(end1 == 0) res = "1";
      else {
        res = num[0]->restoreExpression();
        for (int i = 1; i < end1; i++) {
          res += "*";
          res += num[i]->restoreExpression();
        }
      }
      for (int i = 0; i < end2; i++) {
        std::string el = denom[i]->restoreExpression();
        res += "/";
        if(el[0] != '(') res += '(';
        res += el;
        if(el[0] != '(') res += ')';
      }
      return res;
    }
    void setArgument(functionalExpression* expression, int index) {
      if(index >= 0) {
        num[index] = expression;
      } else {
        denom[-1 - index] = expression;
      }
    }
    double calculate() {
      double res = 1;
      int end1 = num.size();
      int end2 = denom.size();
      for (int i = 0; i < end1; i++) {
        res *= num[i]->calculate();
      }
      for (int i = 0; i < end2; i++) {
        res /= denom[i]->calculate();
      }
      return res;
    }
    void destroy() {
      int end1 = num.size();
      int end2 = denom.size();
      for (int i = 0; i < end1; i++) {
        num[i]->destroy();
        delete num[i];
      }
      for (int i = 0; i < end2; i++) {
        denom[i]->destroy();
        delete denom[i];
      }
    }
  };

  functionalExpression* generate() {
    variable* a = new variable('x');
    quotient* b = new quotient(-3, a);
    constant* c = new constant(6);
    summ* d = new summ(b, c);
    return d;
  }
}
