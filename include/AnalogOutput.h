#ifndef ANALOG_OUTPUT_H
#define ANALOG_OUTPUT_H

#include "Interface.h"
#include "Debug.h"

/**
 * @brief Analog output interface implementation.
 * @details Provides an Interface-derived analog output implementation that stores a resolution and a value (Q).
 */
class AnalogOutput : public Interface {
    public:

        /**
         * @brief Construct a new AnalogOutput object.
         * @details Initializes the analog output with the given resolution and sets the internal value Q to zero.
         * @param res Resolution to use for this analog output. Defaults to Interface::RES_12BIT.
         */
        AnalogOutput(Interface::Resolution res = Interface::RES_12BIT) : resolution(res) {
            Q = 0; 
        }

        /**
         * @brief Get the interface type.
         * @details Returns the type identifier for this interface implementation so callers can distinguish analog outputs from other interface types.
         * @return Interface::Type The type value ANALOG_OUTPUT.
         */
        Interface::Type getType() const override {
                return ANALOG_OUTPUT;
        } 

        /**
         * @brief Set the output value (Q).
         * @details Sets the internal value Q. If the provided value exceeds the maximum representable value for the configured resolution,
         *          the value is clamped to the maximum and a warning is logged.
         * @param q The value to set for this analog output. Range is 0 .. (2^resolution - 1).
         * @return void
         */
        void setQ(uint16_t q) override { 
            if(q > ((1 << resolution) - 1)) {
                WARNING_PRINTLN("[AnalogOutput] WARNING: Attempted to set value " + String(q) + " which exceeds the maximum for resolution " + String(resolution) + " bits. Clamping to max value.");
                Q = (1 << resolution) - 1; // Clamp to max value
            } else {
                Q = q; 
            }
        }
        uint8_t channel;
    private:
        bool valid = false;
        Interface::Resolution resolution=Interface::RES_12BIT;

};

#endif
