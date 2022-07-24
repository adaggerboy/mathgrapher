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
      return 11;
    }
    control getControl(int index) {
      control c;
      switch (index) {
        case 0:
          c = {.field="function", .userDescription="y = ", .type=control::stringT};
        break;
        case 1:
          c = {.field="showGrid", .userDescription="Show grid", .type=control::boolT};
        break;
        case 2:
          c = {.field="showScale", .userDescription="Show scale", .type=control::boolT};
        break;
        case 3:
          c = {.field="aValue", .userDescription="a = ", .type=control::doubleT, .lowLimit=-10, .highLimit=10};
        break;
        case 4:
          c = {.field="bValue", .userDescription="b = ", .type=control::doubleT, .lowLimit=10, .highLimit=-10};
        break;
        case 5:
          c = {.field="showPiScale", .userDescription="Show pi-slace", .type=control::boolT};
        break;
        case 6:
          c = {.field="showSecondFunction", .userDescription="Show second function", .type=control::boolT};
        break;
        case 7:
          c = {.field="secondFunction", .userDescription="y2 = ", .type=control::stringT};
        break;
        case 8:
          c = {.field="scaleProportion", .userDescription="Scale proportion", .type=control::intT, .lowLimit=-8, .highLimit=8};
        break;
        case 9:
          c = {.field="debug", .userDescription="Debug information", .type=control::boolT};
        break;
        case 10:
          c = {.field="weightLine", .userDescription="Double graph line thickness", .type=control::boolT};
        break;
      }
      return c;
    }
    bool setValue(std::string field, bool value) {
      if(field == "showGrid") {
        return setValue(1, value);
      } else if (field == "showScale") {
        return setValue(2, value);
      } else if (field == "showPiScale") {
        return setValue(5, value);
      } else if (field == "showSecondFunction") {
        return setValue(6, value);
      } else if (field == "debug") {
        return setValue(9, value);
      } else if (field == "weightLine") {
        return setValue(10, value);
      }
      return false;
    }
    bool setValue(std::string field, int value) {
      if(field == "scaleProportion") {
        return setValue(8, value);
      }
      return false;
    }
    bool setValue(std::string field, double value) {
      if(field == "aValue") {
        return setValue(3, value);
      } else if (field == "bValue") {
        return setValue(4, value);
      }
      return false;
    }
    bool setValue(std::string field, std::string value) {
      if(field == "function") {
        return setValue(0, value);
      } else if(field == "secondFunction") {
        return setValue(7, value);
      }
      return false;
    }

    double getDoubleValue(std::string field) {
      if(field == "aValue") {
        return getBoolValue(3);
      } else if (field == "bValue") {
        return getBoolValue(4);
      }
      return 0;
    }

    bool getBoolValue(std::string field) {
      if(field == "showGrid") {
        return getBoolValue(1);
      } else if (field == "showScale") {
        return getBoolValue(2);
      } else if (field == "showPiScale") {
        return getBoolValue(5);
      } else if (field == "showSecondFunction") {
        return getBoolValue(6);
      } else if (field == "debug") {
        return getBoolValue(9);
      } else if (field == "weightLine") {
        return getBoolValue(10);
      }
      return false;
    }

    std::string getStringValue(std::string field) {
      if(field == "function") {
        return getStringValue(0);
      } else if(field == "secondFunction") {
        return getStringValue(7);
      }
      return "";
    }

    int getIntValue(std::string field) {
      if(field == "scaleProportion") {
        return getIntValue(8);
      }
      return 0;
    }

    bool setValue(int index, bool value) {
      switch (index) {
        case 1:
          mainGrapher->getGraphParamsTable()->grid = value;
        break;
        case 2:
          mainGrapher->getGraphParamsTable()->scales = value;
        break;
        case 5:
          mainGrapher->getGraphParamsTable()->piscales = value;
        break;
        case 6:
          mainGrapher->getGraphParamsTable()->second = value;
        break;
        case 9:
          mainGrapher->getGraphParamsTable()->debug = value;
        break;
        case 10:
          mainGrapher->getGraphParamsTable()->weight = value;
        break;
        default: return false;
      }
      mainGrapher->change();
      return true;
    }

    bool setValue(int index, int value) {
      switch (index) {
        case 8:
          mainGrapher->getGraphParamsTable()->scprop = value;
        break;
        default: return false;
      }
      mainGrapher->change();
      return true;
    }

    bool setValue(int index, double value) {
      switch (index) {
        case 3:
          if(value < -10 || value > 10) return false;
          globalVarTable.setValue('a', value);
        break;
        case 4:
          globalVarTable.setValue('b', value);
        break;
        default: return false;
      }
      mainGrapher->change();
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
      } else if(index == 7) {
        try {
          functionalExpression* newFunc = generate(value);
          mainGrapher->setSecondFunction(newFunc);
          return true;
        } catch (syntaxError e) {
          return false;
        }
      } else
      return false;
    }

    double getDoubleValue(int index) {
      switch (index) {
        case 3:
          return globalVarTable.getValue('a');
        break;
        case 4:
          return globalVarTable.getValue('b');
        break;
        default: return 0;
      }
    }

    bool getBoolValue(int index) {
      switch (index) {
        case 1: return mainGrapher->getGraphParamsTable()->grid;
        case 2: return mainGrapher->getGraphParamsTable()->scales;
        case 5: return mainGrapher->getGraphParamsTable()->piscales;
        case 6: return mainGrapher->getGraphParamsTable()->second;
        case 9: return mainGrapher->getGraphParamsTable()->debug;
        case 10: return mainGrapher->getGraphParamsTable()->weight;
      }
      return false;
    }

    std::string getStringValue(int index) {
      switch (index) {
        case 0: return mainGrapher->restoreFunctionString();
        case 7: return mainGrapher->restoreSecondFunctionString();
      }
      return "";
    }

    int getIntValue(int index) {
      switch (index) {
        case 8: return mainGrapher->getGraphParamsTable()->scprop;
      }
      return 0;
    }

  };

  controls* initControls(grapher* g) {
    return (controls*)new graphControls(g);
  }
}
