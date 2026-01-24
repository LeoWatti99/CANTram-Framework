#ifndef DIGITAL_OUTPUT_H
#define DIGITAL_OUTPUT_H

#include "Interface.h"
#include "Debug.h"

class DigitalOutput : public Interface {
  public:
    
    Interface::Type getType() const override {
        return DIGITAL_OUTPUT;
    }
  private:

};




#endif