#ifndef OUTPUTDEFINITION_H
#define OUTPUTDEFINITION_H

#include <Arduino.h>
#include "Debug.h"

/**
 * @file OutputDefinition.h
 * @brief Definitions for GPIO/output descriptors and provider interfaces.
 * @details Declares OutputDefinition which describes a single GPIO or shift-register bit and
 *          abstract OutputProvider/PWMOutputProvider interfaces used by CANTramCore to configure
 *          and control outputs.
 */

class OutputProvider; // Forward declaration    

/**
 * @class OutputDefinition
 * @brief Describes an available output (GPIO or shift register bit).
 * @details Holds metadata about an output such as whether it is on a shift register, the pin/bit
 *          identifier, and capabilities (ADC, PWM, input-only). Also stores a pointer to the
 *          OutputProvider responsible for configuring/controlling the physical output.
 */
class OutputDefinition {
    public:
        /**
         * @brief Construct a new OutputDefinition instance.
         * @details Initializes the definition fields describing the output and records the provider
         *          responsible for manipulating the physical pin/bit.
         * @param isShift True if the output is implemented on a shift register rather than a MCU pin.
         * @param pinOrBit Pin number or bit index identifying the physical output.
         * @param adc True if the output supports ADC (analog input) functionality.
         * @param pwm True if the output supports PWM output.
         * @param inputOnly True if the output is input-only (no output capability).
         * @param provider Pointer to the OutputProvider that controls this output.
         */
        OutputDefinition(bool isShift, uint8_t pinOrBit, bool adc, bool pwm, bool inputOnly, OutputProvider* provider) :
            isShift(isShift), pinOrBit(pinOrBit), adc(adc), pwm(pwm), inputOnly(inputOnly), provider(provider)
             {}

        const bool isShift;
        const uint8_t pinOrBit;
        const bool adc;
        const bool pwm;
        const bool inputOnly;
        
        OutputProvider* provider;
        bool initialValue = LOW;

        /**
         * @brief Create a human-readable string describing the OutputDefinition.
         * @details Produces a textual representation suitable for logging that includes the
         *          isShift flag, pin/bit identifier and capability flags.
         * @return String A formatted description of this OutputDefinition.
         */
        String toString() {
            return String("OutputDefinition(isShift=") + (isShift ? "true" : "false") +
                   ", pinOrBit=" + String(pinOrBit) +
                   ", adc=" + (adc ? "true" : "false") +
                   ", pwm=" + (pwm ? "true" : "false") +
                   ", inputOnly=" + (inputOnly ? "true" : "false") + ")";
        }
};

/**
 * @class OutputProvider
 * @brief Abstract interface for providers that control outputs.
 * @details Implementors provide mechanisms to configure outputs (mode), set output state and read state.
 *          Concrete providers adapt MCU GPIO, expanders, or shift registers to the OutputDefinition API.
 */
class OutputProvider {
    public:
        /**
         * @brief Configure an output's direction/mode.
         * @details Configure the physical output referenced by def to the given mode (e.g., INPUT, OUTPUT).
         * @param def Pointer to the OutputDefinition describing the output to configure.
         * @param mode Mode value to set (Arduino-style constants like INPUT/OUTPUT).
         * @return true if configuration succeeded, false on error.
         */
        virtual bool configureOutput(OutputDefinition* def, uint8_t mode) = 0;


        /**
         * @brief Set the logical state of an output.
         * @details Drive the physical output described by def to the requested boolean state.
         * @param def Pointer to the OutputDefinition describing the output to change.
         * @param state Desired logical state (true = HIGH, false = LOW).
         * @return true if the state was successfully applied, false otherwise.
         */
        virtual bool setOutput(OutputDefinition* def, bool state) = 0; //Returns true if successful

        /**
         * @brief Read the current logical state of an output.
         * @details Query the provider for the current state of the specified output. For inputs this
         *          reads the actual level; for outputs the implementation may return the last set state.
         * @param def Pointer to the OutputDefinition to read.
         * @return bool Current logical state (true = HIGH, false = LOW).
         */
        virtual bool getOutput(OutputDefinition* def) = 0; //Returns current state

        /**
         * @brief Enable or disable all outputs managed by this provider.
         * @details Globally enable or disable outputs. Implementation-specific behavior (e.g., power gating). This method is called by the CANTramCore during initialization.
         * @param state true to enable outputs, false to disable.
         */
        virtual void enableOutputs(bool state=true){    //Append this method in derived classes to actually enable the outputs
            if(state == outputsEnabled){    //If state is already as requested, do nothing
                return;
            }
            outputsEnabled = state;
            INFO_PRINTLN("[OutputProvider] Outputs enabled set to: " + String(state)); 
            
        }
        /**
         * @brief Disable all outputs managed by this provider.
         * @details Convenience method to disable outputs. Calls enableOutputs(false).
         */
        virtual void disableOutputs(){
            enableOutputs(false);
        }
    private:
        bool outputsEnabled = false;
    };

/**
 * @class PWMOutputProvider
 * @brief Extended output provider interface for PWM-capable outputs.
 * @details Adds PWM-specific operations on top of OutputProvider for setting PWM duty and registering channels.
 */
class PWMOutputProvider : public OutputProvider {
    public:
        /**
         * @brief Set a PWM value on the specified output.
         * @details Drive the output described by def with a PWM duty corresponding to value. Duty scale/units
         *          are implementation-specific (e.g., 0..4095 for 12-bit resolution).
         * @param def Pointer to the OutputDefinition that supports PWM.
         * @param value PWM duty value to apply.
         * @return true if the PWM value was successfully applied, false otherwise.
         */
        virtual bool setPWM(OutputDefinition* def, uint16_t value) = 0; //Returns true if successful

        /**
         * @brief Register/allocate a PWM channel for the given output.
         * @details Request any internal resources needed to drive PWM on the specified output (timer/channel allocation).
         * @param def Pointer to the OutputDefinition to register for PWM.
         * @return bool true if a PWM channel was successfully registered/allocated, false otherwise.
         */
        virtual bool registerPWMChannel(OutputDefinition* def) = 0; //Returns true if successful
};

#endif