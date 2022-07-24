#ifndef MG_CONTROLS
#define MG_CONTROLS

#include <string>

namespace mg {

  struct control {
    std::string field;
    std::string userDescription;
    enum controlType {boolT, intT, doubleT, stringT, signalT} type;
    double lowLimit, highLimit;
  };

  class controls {
  public:
    virtual int getControlsCount() = 0;
    virtual control getControl(int index) = 0;
    virtual bool setValue(std::string field, bool value) = 0;
    virtual bool setValue(std::string field, int value) = 0;
    virtual bool setValue(std::string field, double value) = 0;
    virtual bool setValue(std::string field, std::string value) = 0;
    virtual bool sendSignal(std::string field) = 0;
    virtual bool setValue(int index, bool value) = 0;
    virtual bool setValue(int index, int value) = 0;
    virtual bool setValue(int index, double value) = 0;
    virtual bool setValue(int index, std::string value) = 0;
    virtual bool sendSignal(int index) = 0;
    virtual double getDoubleValue(std::string field) = 0;
    virtual bool getBoolValue(std::string field) = 0;
    virtual std::string getStringValue(std::string field) = 0;
    virtual int getIntValue(std::string field) = 0;
    virtual double getDoubleValue(int index) = 0;
    virtual bool getBoolValue(int index) = 0;
    virtual std::string getStringValue(int index) = 0;
    virtual int getIntValue(int index) = 0;
  };
}

#endif /* end of include guard: MG_CONTROLS */
