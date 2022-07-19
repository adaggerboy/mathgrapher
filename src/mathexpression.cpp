#include "mathexpression.hpp"

#include <vector>
#include <cmath>
#include <iostream>

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
    else extended[name] = value;
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
      std::string el = arg->restoreExpression();
      if(quot == 1) return el;
      if(quot == -1) return "-" + el;
      if(el[0] >= 0 && el[0] <= 9) return std::to_string(quot) + "*" + arg->restoreExpression();
      return std::to_string(quot) + arg->restoreExpression();
    }
    void setArgument(functionalExpression* expression, int index = 0) {
      arg = expression;
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
      args.push_back(expression);
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
      args.~vector();
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
        num.push_back(expression);
      } else {
        denom.push_back(expression);
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
      num.~vector();
      denom.~vector();
    }
  };

  class power : public functionalExpression {
    functionalExpression* base;
    functionalExpression* arg;
  public:
    power() {}
    power(functionalExpression* a, functionalExpression* b) {
      arg = a;
      base = b;
    }
    std::string restoreExpression() {
      std::string el = arg->restoreExpression();
      std::string res = "";
      if(el[0] != '(') res += "(";
      res += el;
      if(el[0] != '(') res += ")";
      res += "^";
      el = base->restoreExpression();
      if(el[0] != '(') res += "(";
      res += el;
      if(el[0] != '(') res += ")";
      return res;
    }
    void setArgument(functionalExpression* expression, int index = 0) {
      if(index == 0) {
        arg = expression;
      } else {
        base = expression;
      }
    }
    double calculate() {
      return pow(arg->calculate(), base->calculate());
    }
    void destroy() {
      base->destroy();
      arg->destroy();
      delete base;
      delete arg;
    }
  };

  class logarithm : public functionalExpression {
    functionalExpression* base;
    functionalExpression* arg;
  public:
    logarithm() {}
    logarithm(functionalExpression* a, functionalExpression* b) {
      base = a;
      arg = b;
    }
    std::string restoreExpression() {
      std::string res = "log";
      std::string el = base->restoreExpression();
      if(el[0] != '(') res += "(";
      res += el;
      if(el[0] != '(') res += ")";
      el = arg->restoreExpression();
      if(el[0] != '(') res += "(";
      res += el;
      if(el[0] != '(') res += ")";
      return res;
    }
    void setArgument(functionalExpression* expression, int index = 0) {
      if(index == 0) {
        arg = expression;
      } else {
        base = expression;
      }
    }
    double calculate() {
      return log(arg->calculate()) / log(base->calculate());
    }
    void destroy() {
      base->destroy();
      arg->destroy();
      delete base;
      delete arg;
    }
  };

  class definedFunction : public functionalExpression {
    functionalExpression* arg;
  public:
    enum functionType {sinF, cosF, expF, lnF};
  private:
    functionType type;
  public:
    definedFunction(functionType t) {
      type = t;
    }
    definedFunction(functionType t, functionalExpression* a) {
      type = t;
      arg = a;
    }
    std::string restoreExpression() {
      std::string res = "";
      switch (type) {
        case sinF:
          res += "sin";
        break;
        case cosF:
          res += "cos";
        break;
        case expF:
          res += "exp";
        break;
        case lnF:
          res += "ln";
        break;
      }
      std::string el = arg->restoreExpression();
      if(el[0] != '(') res += "(";
      res += el;
      if(el[0] != '(') res += ")";
      return res;
    }
    void setArgument(functionalExpression* expression, int index = 0) {
      arg = expression;
    }
    double calculate() {
      switch (type) {
        case sinF: return sin(arg->calculate());
        case cosF: return cos(arg->calculate());
        case expF: return exp(arg->calculate());
        case lnF: return log(arg->calculate());
        default: return 0;
      }
    }
    void destroy() {
      arg->destroy();
      delete arg;
    }
  };


  syntaxError::syntaxError(int c, int p) {
    code = c;
    position = p;
  }

  std::string syntaxError::what() {
    std::string res = syntaxErrors[code];
    res += " at ";
    res += std::to_string(position);
    return res;
  }


  //not assigned, constant, identifier(v), sign, braces
  enum tokenType {n, c, v, s, b};

  //single level token node
  struct sltnode {
    tokenType type;
    std::string expr;
    sltnode* next;
    sltnode* prev;
    int position;
  };

  void addnode(sltnode*& begin, sltnode*& end, sltnode* cur) {
    if(begin && end) {
      cur->prev = end;
      end->next = cur;
      cur->next = NULL;
      end = cur;
    } else {
      begin = end = cur;
      begin->next = end->prev = NULL;
    }
  }

  functionalExpression* generate(std::string expr, int dbgOffset);

  inline functionalExpression* parseVPattern(sltnode* c1, sltnode* c2 = NULL, sltnode* c3 = NULL) {
    if(c2 == NULL) {
      if(c1->expr.length() == 1) return new variable(c1->expr[0]);
      else return new extendedVariable(c1->expr);
    } else if (c3 == NULL) {
      if(c1->expr == "sin") {
        return new definedFunction(definedFunction::sinF, generate(c2->expr, c2->position));
      } else if(c1->expr == "cos") {
        return new definedFunction(definedFunction::cosF, generate(c2->expr, c2->position));
      } else if(c1->expr == "exp") {
        return new definedFunction(definedFunction::expF, generate(c2->expr, c2->position));
      } else if(c1->expr == "ln") {
        return new definedFunction(definedFunction::lnF, generate(c2->expr, c2->position));
      } else throw syntaxError(5, c1->position);
    } else {
      if(c1->expr == "log") {
        if(c2->type == c) return new logarithm(new constant(std::stod(c2->expr)), generate(c3->expr, c3->position));
        else return new logarithm(generate(c2->expr, c2->position), generate(c3->expr, c3->position));
      } else throw syntaxError(6, c1->position);
    }
  }

  inline functionalExpression* parsePattern(sltnode* begin, sltnode* end) {
    std::vector<functionalExpression*> comb;
    functionalExpression* tmp = NULL;
    int mode = 0, vmode = 0;
    sltnode *c1, *c2, *c3;
    c1 = c2 = c3 = NULL;

    for (sltnode* cur = begin; cur; cur = cur->next) {
      if(cur->type == c) {

        if(mode == 0) {
          tmp = new constant(std::stod(cur->expr));
          mode = 1;
        } else if(mode == 1) {
          throw syntaxError(2, cur->position);
        } else if(mode == 2) {
          tmp = new power(tmp, new constant(std::stod(cur->expr)));
          mode = 1;
        } else if(mode == 3 || mode == 4) {
          if(vmode == 0) {
            vmode = 1;
            c2 = cur;
          } else throw syntaxError(2, cur->position);
        }

      } else if(cur->type == b) {

        if(mode == 0 || mode == 1) {
          if(tmp) comb.push_back(tmp);
          tmp = generate(cur->expr, cur->position);
          mode = 1;
        } else if(mode == 2) {
          tmp = new power(tmp, generate(cur->expr, cur->position));
          mode = 1;
        } else if(mode == 3 || mode == 4) {
          if(vmode == 0) {
            vmode = 1;
            c2 = cur;
          } else if (vmode == 1) {
            vmode = 2;
            c3 = cur;
          } else {
            if(mode == 4) {
              comb.push_back(new power(tmp, parseVPattern(c1, c2, c3)));
            } else comb.push_back(parseVPattern(c1, c2, c3));
            c1 = c2 = c3 = NULL;
            tmp = generate(cur->expr, cur->position);
            mode = 1;
          }

        }

      } else if(cur->type == s) {

        if(mode == 0) throw syntaxError(2, cur->position);
        else if(mode == 1) {
          if(cur->expr[0] == '^') mode = 2;
        } else if (mode == 2) throw syntaxError(4, cur->position);
        else if (mode == 3) {
          tmp = parseVPattern(c1, c2, c3);
          c1 = c2 = c3 = NULL;
          if(cur->expr[0] == '^') mode = 2;
        } else if (mode == 4) throw syntaxError(4, cur->position);
      } else if(cur->type == v) {

        if(mode == 0) {
          mode = 3;
        } else if(mode == 1) {
          if(tmp) comb.push_back(tmp);
          mode = 3;
        } else if(mode == 2) {
          mode = 4;
        } else if(mode == 3) {
          comb.push_back(parseVPattern(c1, c2, c3));
          c1 = c2 = c3 = NULL;
          mode = 3;
        } else if(mode == 4) {
          comb.push_back(new power(tmp, parseVPattern(c1, c2, c3)));
          c1 = c2 = c3 = NULL;
          mode = 3;
        }
        vmode = 0;
        c1 = cur;

      }
      if(cur == end) break;
    }

    if(mode == 0) return new constant(0);
    else if(mode == 1) {
      comb.push_back(tmp);
    } else if(mode == 2) {
      throw syntaxError(3, end->position + end->expr.length());
    } else if(mode == 3) {
      comb.push_back(parseVPattern(c1, c2, c3));
    } else if(mode == 4) {
      comb.push_back(new power(tmp, parseVPattern(c1, c2, c3)));
    }

    if(comb.size() == 0) return new constant(0);
    else if(comb.size() == 1) return comb[0];
    else {
      fractionCombination* res = new fractionCombination;
      for (int i = 0; i < comb.size(); i++) {
        res->setArgument(comb[i], 0);
      }
      return res;
    }

  }

  functionalExpression* generate(std::string expr, int dbgOffset) {
    sltnode *begin, *end;
    begin = end = NULL;
    std::string buffer = "";
    tokenType type = n;
    bool point = false;
    int level = 0;
    int position;

    int i;

    for (i = 0; i < expr.length(); i++) {
      if(type == n) {
        if(expr[i] == ' ') continue;
        if((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.') {
          if(expr[i] == '.') point = true;
          else point = false;
          type = c;
          buffer += expr[i];
          position = i + dbgOffset;
        } else if(expr[i] == '*' || expr[i] == '/' || expr[i] == '+' || expr[i] == '-' || expr[i] == '^') {
          buffer += expr[i];
          addnode(begin, end, new sltnode{.type = s, .expr = buffer, .position = i + dbgOffset});
          buffer = "";
        } else if((expr[i] >= 'a' && expr[i] <= 'z') || (expr[i] >= 'A' && expr[i] <= 'Z')) {
          type = v;
          buffer += expr[i];
          position = i + dbgOffset;
        } else if(expr[i] == '(') {
          type = b;
          position = i + dbgOffset;
        } else throw syntaxError(1, i);

      } else if(type == c) {
        if(expr[i] >= '0' && expr[i] <= '9') {
          buffer += expr[i];
        } else if(expr[i] == '.') {
          if(point) throw syntaxError(0, i);
          point = true;
          buffer += expr[i];
        } else {
          addnode(begin, end, new sltnode{.type = type, .expr = buffer, .position = position});
          buffer = "";
          type = n;
          i--;
        }

      } else if(type == v) {

        if((expr[i] >= 'a' && expr[i] <= 'z') || (expr[i] >= 'A' && expr[i] <= 'Z')) {
          buffer += expr[i];
        } else {
          addnode(begin, end, new sltnode{.type = type, .expr = buffer, .position = position});
          buffer = "";
          type = n;
          i--;
        }

      } else if(type == b) {

        if (expr[i] == ')') {
          if(level == 0) {
            addnode(begin, end, new sltnode{.type = type, .expr = buffer, .position = position});
            buffer = "";
            type = n;
          } else {
            level--;
            buffer += expr[i];
          }
        } else if (expr[i] == '(') {
          level++;
          buffer += expr[i];
        } else {
          buffer += expr[i];
        }

      }
    }
    if(type == c) {
      addnode(begin, end, new sltnode{.type = type, .expr = buffer, .position = position});
    } else if(type == v) {
      addnode(begin, end, new sltnode{.type = type, .expr = buffer, .position = position});
    } else if(type == b) {
      throw syntaxError(3, expr.length() + dbgOffset);
    }


    int mode = 0;
    std::vector<functionalExpression*> traces1; // . + . - ...
    std::vector<functionalExpression*> traces2; // . * . * ...
    std::vector<functionalExpression*> traces3; // / . / ...
    functionalExpression* tmp = NULL;
    functionalExpression* t = NULL;
    bool sign, sign2;
    bool denom, denom2;


    sign = denom = false;
    sign2 = denom2 = false;

    sltnode *pbegin = begin, *pend = begin;

    for (sltnode* cur = begin; cur; cur = cur->next) {
      if(cur->type == s && cur->expr[0] != '^') {

        if(pbegin == cur) {
          if(cur->expr[0] != '-') throw syntaxError(2, cur->position);
          else {
            sign = true;
            pbegin = pend = cur->next;
            continue;
          }
        } else {
          if(mode == 1) {
            if(sign2) tmp = new quotient(-1, tmp);
            if(denom2) traces3.push_back(tmp);
            else traces2.push_back(tmp);
            t = new fractionCombination;
            for (int i = 0; i < traces2.size(); i++) {
              t->setArgument(traces2[i], 0);
            }
            for (int i = 0; i < traces3.size(); i++) {
              t->setArgument(traces3[i], -1);
            }
            traces2.clear();
            traces3.clear();
            traces1.push_back(t);
          } else if (mode == 2) {
            if(sign2) tmp = new quotient(-1, tmp);
            if(denom2) traces3.push_back(tmp);
            else traces2.push_back(tmp);
          } else if (mode == 0) {

            if(tmp) {
              if(sign2) tmp = new quotient(-1, tmp);
              traces1.push_back(tmp);
            }

          }
          tmp = parsePattern(pbegin, pend);
        }

        pbegin = pend = cur->next;
        sign2 = sign;
        denom2 = denom;
        sign = denom = false;
        if(cur->expr[0] == '+') {
          if(mode == 2) mode = 1;
          else if(mode == 1) mode = 0;
        } else if(cur->expr[0] == '-') {
          sign = true;
          if(mode == 2) mode = 1;
          else if(mode == 1) mode = 0;
        } else if(cur->expr[0] == '*') {
          mode = 2;
        } else if(cur->expr[0] == '/') {
          denom = true;
          mode = 2;
        }

      } else pend = cur;
    }

    if(!pbegin) throw syntaxError(3, dbgOffset + expr.length());
    if(mode == 0){
      if(tmp) {
        if(sign2) tmp = new quotient(-1, tmp);
        traces1.push_back(tmp);
      }
      if(sign) traces1.push_back(new quotient(-1, parsePattern(pbegin, pend)));
      else traces1.push_back(parsePattern(pbegin, pend));
    } else if(mode == 1) {
      if(sign2) tmp = new quotient(-1, tmp);
      if(denom2) traces3.push_back(tmp);
      else traces2.push_back(tmp);
      t = new fractionCombination;
      for (int i = 0; i < traces2.size(); i++) {
        t->setArgument(traces2[i], 0);
      }
      for (int i = 0; i < traces3.size(); i++) {
        t->setArgument(traces3[i], -1);
      }
      traces1.push_back(t);
      if(sign) tmp = new quotient(-1, parsePattern(pbegin, pend));
      else tmp = parsePattern(pbegin, pend);
      traces1.push_back(tmp);


    } else if(mode == 2) {
      if(sign2) tmp = new quotient(-1, tmp);
      if(denom2) traces3.push_back(tmp);
      else traces2.push_back(tmp);
      if(sign) tmp = new quotient(-1, parsePattern(pbegin, pend));
      else tmp = parsePattern(pbegin, pend);
      if(denom) traces3.push_back(tmp);
      else traces2.push_back(tmp);
      t = new fractionCombination;
      for (int i = 0; i < traces2.size(); i++) {
        t->setArgument(traces2[i], 0);
      }
      for (int i = 0; i < traces3.size(); i++) {
        t->setArgument(traces3[i], -1);
      }
      traces1.push_back(t);
    }

    if(traces1.size() == 0) return new constant(0);
    else if(traces1.size() == 1) return traces1[0];
    else {
      summ* s = new summ();
      for (int i = 0; i < traces1.size(); i++) {
        s->setArgument(traces1[i], 0);
      }
      return s;
    }
  }
}
