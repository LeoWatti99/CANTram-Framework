#ifndef INTERFACE_BASE_H
#define INTERFACE_BASE_H
#include <Arduino.h>
#include "Debug.h"


class Interface {
  public:
    enum Resolution {
            RES_8BIT = 8,
            RES_10BIT = 10,
            RES_12BIT = 12,
            RES_16BIT = 16
        };
    

    enum Type {
        DIGITAL_INPUT,
        DIGITAL_OUTPUT,
        ANALOG_INPUT,
        ANALOG_OUTPUT,
        RELAIS,
        BUS
    };
    // Overload setQ for bool and float_t to auto-cast to Value
    virtual void setQ(uint16_t q) { Q = q; } // Accepts float_t, sets analog
    virtual uint16_t getQ() const { return Q; }
    virtual Type getType() const = 0;           //Type of the interface
    
    void rename(const String& newName) { 
      String oldName = name;
      name = newName;
      INFO_PRINTLN("[Interface] Interface renamed " + oldName + " to " + name);
    }
    template <typename T> 
    T as() {return static_cast<T>(this);}

    void validate() { _valid = true; }
    void validate(bool state) { _valid = state; }
    void invalidate() { _invalid = true; }
    bool isValid() { return _valid && !_invalid; }
    bool isInvalid() { return _invalid; }

    String getName() const { return name; }
  protected:
    String  name;
    Interface() = default;
    uint16_t Q = 0;
    bool _valid = false;
    bool _invalid = false;
    
    
    // Protected members here
};

#endif