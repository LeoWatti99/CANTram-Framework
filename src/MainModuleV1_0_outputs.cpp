/**
 * @file MainModuleV1_0_outputs.cpp
 * @author your name (you@domain.com)
 * @brief This file contains all output related functionallity of the MainmoduleV1_0. This includes native GPIO pins and shift register pins.
 * @version 0.1
 * @date 2025-10-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "MainModuleV1_0.h"
#include <Arduino.h>
#include "CANTramCore.h"
#include <SPI.h>
#include "OutputDefinition.h"
#include "Debug.h"
#include "HardwareResource.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"
#include "esp_adc_cal.h"
#include "Adafruit_MCP23X17.h"

/**
 * @brief Construct a new MainModuleV1_0::MainModuleV1_0 object
 * 
 * @details This constructor explicitly calls the ESP32_I2CCore constructor with I2C bus 0 so Wire instead of Wire1 is used.
 */
MainModuleV1_0::MainModuleV1_0() : i2cCore(ESP32_I2CCore::ESP_I2C_NUM_0) {    
}


/**
 * @brief Configures the output for a specific pin or bit.
 * 
 * @param def GPIO number or shiftRegister output ID of desired output
 * @param mode (Optional) Specify the direction of the pin. Default is OUTPUT
 * @return true, if the cofiguration was completed successfully
 * @return false, if error were encountered during the configuration. See output log for more details
 */
bool MainModuleV1_0::configureOutput(OutputDefinition* def, uint8_t mode) {
    if(def->isShift) {
        #ifdef MAINMODULE_NO_SHIFTREGISTER
        DEV_ERROR_PRINTLN("[MainModuleV1_0] Shift register functionality disabled. Check your code for the Makro \"MAINMODULE_NO_SHIFTREGISTER\". This macro prevents the usage of the shiftregister");
        return true;
        #endif
        //Check if shift register is accessible
        if(!startShiftRegister()){
            ERROR_PRINTLN("[MainModuleV1_0] Output "+ def->toString()+ "couldn't be configured.");
            return false;
        }
        shiftRegister.pinMode(def->pinOrBit, mode);
        INFO_PRINTLN("[MainModuleV1_0] Output "+def->toString() + " configured in mode "+String(mode));
        
        
        // Example: ShiftRegister.configure(def.pinOrBit, mode);
        //ERROR_PRINTLN("[MainModuleV1_0] ERROR: Shift register configure output not implemented yet for output "+ String(def->toString()));
        return true;
    }
    if(def->inputOnly) {
        ERROR_PRINTLN("[MainModuleV1_0] ERROR: Attempted to configure input-only output "+ String(def->toString()));
        return false;
    }
    gpio_reset_pin(static_cast<gpio_num_t>(def->pinOrBit));
    gpio_set_direction(static_cast<gpio_num_t>(def->pinOrBit), static_cast<gpio_mode_t>(mode));
    INFO_PRINTLN("[MainModuleV1_0] INFO: Configured GPIO pin " + String(def->pinOrBit) + " as OUTPUT");
    return true;
}

/**
 * @brief Set the logic level on a pin of the module
 * 
 * @param def Output to set the state
 * @param state logic level HIGH or LOW
 * @return true, if logic level was set correctly
 */
bool MainModuleV1_0::setOutput(OutputDefinition* def, bool state) {
    if(def->isShift) {
        //Check if shift register is accessible
        #ifdef MAINMODULE_NO_SHIFTREGISTER
        DEV_ERROR_PRINTLN("[MainModuleV1_0] Shift register functionality disabled. Check your code for the Makro \"MAINMODULE_NO_SHIFTREGISTER\". This macro prevents the usage of the shiftregister");
        return true;
        #endif

        
        if(!_shiftregisterStatus==STARTED){
            ERROR_PRINTLN("[MainModuleV1_0] Shift register not started. Cannot set output "+ def->toString());
            return false;
        }  
        
        //Set new state of the desired pin
        shiftRegister.digitalWrite(def->pinOrBit,state);

        //Check if pin was set correctly
        if(shiftRegister.digitalRead(def->pinOrBit)!=state){
            ERROR_PRINTLN("[MainModuleV1_0] Failed to write logic level "+ String(state)+" to output "+ def->toString());
            return false;
        }
        // Handle shift register output
        // Example: ShiftRegister.setOutput(def.pinOrBit, state);
        //DEV_ERROR_PRINTLN("[MainModuleV1_0] ERROR: Shift register output not implemented yet for output "+ String(def->toString()));
        return true;
    } else {
        // Handle direct GPIO output
        gpio_set_level(static_cast<gpio_num_t>(def->pinOrBit), state );
        DEBUG_PRINTLN("[MainModuleV1_0] INFO: Setting GPIO pin " + String(def->pinOrBit) + " to " + String(state));
        return true; //Assume always successful for now
    }
}
/**
 * @brief Get the current state of a digital output
 * 
 * @param def Output definition containing the pin information
 * @return true if the output state was retrieved successfully
 * @return false if an error occurred
 */
bool MainModuleV1_0::getOutput(OutputDefinition* def) {
    if(def->isShift) {
        //Check if shift register is accessible
        #ifdef MAINMODULE_NO_SHIFTREGISTER
        DEV_ERROR_PRINTLN("[MainModuleV1_0] Shift register functionality disabled. Check your code for the Makro \"MAINMODULE_NO_SHIFTREGISTER\". This macro prevents the usage of the shiftregister");
        return false;
        #endif
        //Check if shift register is accessible
        if(!_shiftregisterStatus==STARTED) return false;

        //Return the logic level of the pin
        
        bool result = shiftRegister.digitalRead(def->pinOrBit);
        DEBUG_PRINTLN("[MainModule] Shiftregister output "+ def->toString() + " is "+String(result));
        return result;
        // Handle shift register output
        // Example: ShiftRegister.getOutput(def.pinOrBit);
        //ERROR_PRINTLN("[MainModuleV1_0] ERROR: Shift register get output not implemented yet for output "+ String(def->toString()));
        
    } else {
        // Handle direct GPIO output
        int state = gpio_get_level(static_cast<gpio_num_t>(def->pinOrBit));
        return (state == HIGH);
    }
}

/**
 * @brief Starts the integrated shift register in I2C mode.
 * 
 * @return true, is started successfully
 * 
 */
bool MainModuleV1_0::startShiftRegister(){
        //Skip function call if already started
    if((_shiftregisterStatus==STARTED)){
        return true;
    }

    //Check if initialization failed previously
    if(_shiftregisterStatus==FAILED){
        return false;
    }

    //Check if I2C is initialized
    
    //Start in I2C mode
    
    bool result = shiftRegister.begin_I2C(SHIFTREGISTER_I2C_ADDRESS,&i2cCore.getWireInstance());
    if(!result) {
        ERROR_PRINTLN("[MainModuleV1_0] Failed to start shift register. Additional outputs in expander bus wont be usable");
        _shiftregisterStatus = FAILED;
        return false;
    }
    INFO_PRINTLN("[MainModuleV1_0] Shift register started successfully.");
    _shiftregisterStatus = STARTED;
    return result;
}

/**
 * @brief Resets the shift register to its initial state.
 * 
 * @return true if the reset was successful
 * @return false if the reset failed
 */
bool MainModuleV1_0::resetShiftRegister(){

    #ifdef MAINMODULE_NO_SHIFTREGISTER
    DEV_ERROR_PRINTLN("[MainModuleV1_0] Shift register functionality disabled. Checkyour code for the Makro \"MAINMODULE_NO_SHIFTREGISTER\". This macro prevents the usage of the shiftregister");
    return true;
    #endif
    //restore default values
    INFO_PRINTLN("[MainModuleV1_0] Resetting shift register to default state...");
    bool result = true;
    
    if(!_shiftregisterStatus==STARTED){
        ERROR_PRINTLN("[MainModuleV1_0] Cannot reset shift register because it is not started.");
        return false;
    }
    
    for(uint8_t pin=0; pin <16; pin++){
        DEBUG_PRINTLN("[MainModuleV1_0] Resetting shift register pin "+String(pin)+" to LOW and INPUT");
        shiftRegister.digitalWrite(pin,LOW);
        shiftRegister.pinMode(pin,INPUT);
        result &= !shiftRegister.digitalRead(pin); //Verify pin is LOW
    }

    //Debug output
    if(result) INFO_PRINTLN("[MainModuleV1_0] Shift register reset successfully.");
    else ERROR_PRINTLN("[MainModuleV1_0] Shift register reset failed.");

    _shiftregisterStatus = NOT_STARTED;
    return result;
}


void MainModuleV1_0::enableOutputs(bool state){
    OutputProvider::enableOutputs(state); //Call base class implementation
    shiftRegister.pinMode(GENERAL_EN,OUTPUT);
    shiftRegister.digitalWrite(GENERAL_EN,state);
    INFO_PRINTLN("[MainModuleV1_0] General output enable set to: " + String(state));
}
  