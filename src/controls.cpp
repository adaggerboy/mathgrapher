#include "controls.hpp"
#include "mathgrapher.hpp"

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
        mainGrapher->getGraphParamsTable()->grid = value;
      } else if (field == "showScale") {
        mainGrapher->getGraphParamsTable()->scales = value;
      }
      mainGrapher->change();
      return true;
    }
    bool setValue(std::string field, int value) {
      return true;

    }
    bool setValue(std::string field, double value) {
      return true;

    }
    bool setValue(std::string field, std::string value) {
      return true;
    }

    double getDoubleValue(std::string field) {

    }

    bool getBoolValue(std::string field) {
      if(field == "showGrid") {
        return mainGrapher->getGraphParamsTable()->grid;
      } else if (field == "showScale") {
        return mainGrapher->getGraphParamsTable()->scales;
      }
      return false;
    }

    std::string getStringValue(std::string field) {

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

      return true;
    }

  };

  controls* initControls(grapher* g) {
    return (controls*)new graphControls(g);
  }
}
