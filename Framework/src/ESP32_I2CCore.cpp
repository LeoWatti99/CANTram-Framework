#include <Arduino.h>
#include "ESP32_I2CCore.h"
#include "Debug.h"
ESP32_I2CCore::ESP32_I2CCore(uint8_t i2c_num){
    // Constructor
    _i2c_num=i2c_num;

    switch(i2c_num) {
        case ESP_I2C_NUM_0:
            _wire=&Wire;
            
            break;
        case ESP_I2C_NUM_1:
            _wire=&Wire1;
            break;
        default:
            CRITICAL_ERROR_PRINTLN("[ESP32_I2CCore] ERROR: Invalid I2C port number " + String(i2c_num) + ". Only 0 and 1 are supported.");
            while(1); // Halt execution       
    }
    
}
bool ESP32_I2CCore::begin(){
    if(_i2cStatus == I2C_STARTED) {
        DEBUG_PRINTLN("[ESP32_I2CCore] I2C port " + String(_i2c_num) + " already started.");
        return true; // Already started
    }
    _wire->setPins(_sdaPin, _sclPin);
    _wire->setBufferSize(_bufferSize);   
    bool result = _wire->begin();
    _wire->setClock(_frequency);
    if(!result) {
        ERROR_PRINTLN("[ESP32_I2CCore] ERROR: Failed to initialize I2C port " + String(_i2c_num) + ".");
        return false;
    }
    DEBUG_PRINTLN("[ESP32_I2CCore] I2C port " + String(_i2c_num) + " initialized with SDA pin " + String(_sdaPin) + ", SCL pin " + String(_sclPin) + ", frequency " + String(_frequency) + " Hz, buffer size " + String(_bufferSize) + " bytes.");
    _i2cStatus = I2C_STARTED;
    return true;
}


bool ESP32_I2CCore::reset() {
    _wire->end();
    I2CCore::reset();
    
    return true;
}
/*
bool ESP32_I2CCore::write(uint8_t address, uint8_t* data, size_t length) {
    
    DEBUG_PRINTLN("[ESP32_I2CCore] Writing to I2C address " + String(address) + " on port " + String(_i2cNum));
    // Following https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/peripherals/i2c.html#master-write
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Create I2C command sequence 
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    uint8_t modifiedAddress = WRITE_ADDRESS(address);
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, modifiedAddress, _useACK)); // Address + Write bit
    ESP_ERROR_CHECK(i2c_master_write(cmd, data, length, _useACK)); // Data
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    //begin transmission
    esp_err_t result = i2c_master_cmd_begin(_i2cNum, cmd, pdMS_TO_TICKS(1000)); // 1 second timeout
    i2c_cmd_link_delete(cmd);

    if(result != ESP_OK) {
        ERROR_PRINTLN("[ESP32_I2CCore] ERROR: I2C write to address " + String(address) + " failed with error code " + String(result));
        return false; // Return false if write failed
    
    return true; // Return true if write was successful, false otherwise
}}*/