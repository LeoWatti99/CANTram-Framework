#ifndef ESP32_I2CCORE_H
#define ESP32_I2CCORE_H

#include "I2CCore.h"
#include <Arduino.h>
#include <Debug.h>
#include "driver/i2c.h"
#include <Wire.h>

#define WRITE_ADDRESS(address) ((address << 1) | I2C_MASTER_WRITE)
#define READ_ADDRESS(address)  ((address << 1) | I2C_MASTER_READ)

class ESP32_I2CCore : public I2CCore {
  public:
    ESP32_I2CCore(uint8_t i2c_num);
    bool reset() override;
    bool begin() override; 
    void beginTransmission(uint8_t address) override {
        _wire->beginTransmission(address);
    };
    uint8_t endTransmission() override {
        return _wire->endTransmission();
    };
    size_t write(uint8_t data) override {
        return _wire->write(data);
    };
    size_t write(const uint8_t *data, size_t quantity) override {
        return _wire->write(data, quantity);
    };
    size_t requestFrom(uint8_t address, size_t len, bool stopBit) override {
        return _wire->requestFrom(address, len, stopBit);
    };
    int available() override {
        return _wire->available();
    };
    int read() override {
        return _wire->read();
    };
    int peek() override {
        return _wire->peek();
    };
    void flush() override {
        _wire->flush();
    };
    TwoWire& getWireInstance() {
        return *_wire;
    };

    static constexpr uint8_t ESP_I2C_NUM_0 = 0;
    static constexpr uint8_t ESP_I2C_NUM_1 = 1;

  protected:
    // Protected members here
    private:
    TwoWire* _wire;
    uint8_t _i2c_num;
    
};

#endif