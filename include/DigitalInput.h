#ifndef DIGITAL_INPUT_H
#define DIGITAL_INPUT_H

#include "Interface.h"
#include "Debug.h"

class DigitalInput : public Interface {
  public:
    DigitalInput() =default;
    enum Pull {
        NONE,
        UP,
        DOWN
    };
    
    Interface::Type getType() const override {
        return DIGITAL_INPUT;
    }

    
    
 
  private:
    Pull pull;

};

#endif