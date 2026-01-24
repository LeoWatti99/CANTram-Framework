#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include "Interface.h"
#include "Debug.h"

class AnalogInput : public Interface {
    public:
        
        Interface::Type getType() const override {
                return ANALOG_INPUT;
        }
        AnalogInput(Interface::Resolution res = Interface::RES_12BIT) : resolution(res) {
            Q = 0; 
        }
        Interface::Resolution getResolution() const {
            return resolution;
        }
        void setQ(uint16_t q) override { 
            if(q > ((1 << resolution) - 1)) {
                WARNING_PRINTLN("[AnalogInput] WARNING: Attempted to set value " + String(q) + " which exceeds the maximum for resolution " + String(resolution) + " bits. Clamping to max value.");
                Q = (1 << resolution) - 1; // Clamp to max value
            } else {
                Q = q; 
                DEBUG_PRINTLN("[AnalogInput] " + name + " set to " + String(Q));
            }
        } 
    private:
        Interface::Resolution resolution = Interface::RES_12BIT;
    

};

#endif
