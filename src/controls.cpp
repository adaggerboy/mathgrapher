#include "controls.hpp"
#include "mathgrapher.hpp"
#include "mathexpression.hpp"

#include <iostream>

namespace mg {
  class graphControls : public controls {
    grapher* mainGrapher;
  public:
    graphControls(grapher* g) {
      mainGrapher = g;
    }
    int getControlsCount() {
      return 3;
    }
    control getControl(int index) {
      control c;
      switch (index) {
        case 0:
          c = {.field="funct", .userDescription="Function", .type=control::stringT};
        break;
        case 1:
          c = {.field="showGrid", .userDescription="Show grid", .type=control::boolT};
        break;
        case 2:
          c = {.field="showScale", .userDescription="Show scale", .type=control::boolT};
        break;
      }
      return c;
    }
    bool setValue(std::string field, bool value) {
      if(field == "showGrid") {
        return setValue(1, value);
      } else if (field == "showScale") {
        return setValue(2, value);
      }
      return false;
    }
    bool setValue(std::string field, int value) {
      return true;

    }
    bool setValue(std::string field, double value) {
      return true;

    }
    bool setValue(std::string field, std::string value) {
      if(field == "funct") {
        return setValue(0, value);
      }
      return false;
    }

    double getDoubleValue(std::string field) {

    }

    bool getBoolValue(std::string field) {
      if(field == "showGrid") {
        return getBoolValue(1);
      } else if (field == "showScale") {
        return getBoolValue(1);
      }
      return false;
    }

    std::string getStringValue(std::string field) {
      if(field == "funct") {
        return getStringValue(0);
      }
      return "";
    }

    int getIntValue(std::string field) {

    }

    bool setValue(int index, bool value) {
      switch (index) {
        case 1:
          mainGrapher->getGraphParamsTable()->grid = value;
        break;
        case 2:
          mainGrapher->getGraphParamsTable()->scales = value;
        break;
        default: return false;
      }
      mainGrapher->change();
      return true;
    }

    bool setValue(int index, int value) {

      return true;
    }

    bool setValue(int index, double value) {

      return true;
    }

    bool setValue(int index, std::string value) {
      if(index == 0) {
        try {
          functionalExpression* newFunc = generate(value);
          mainGrapher->setFunction(newFunc);
          return true;
        } catch (syntaxError e) {
          return false;
        }
      }
      return false;
    }

    double getDoubleValue(int index) {

    }

    bool getBoolValue(int index) {
      switch (index) {
        case 1: return mainGrapher->getGraphParamsTable()->grid;
        case 2: return mainGrapher->getGraphParamsTable()->scales;
      }
      return false;
    }

    std::string getStringValue(int index) {
      switch (index) {
        case 0: return mainGrapher->restoreFunctionString();
      }
      return "";
    }

    int getIntValue(int index) {

    }

  };

  controls* initControls(grapher* g) {
    return (controls*)new graphControls(g);
  }
}
