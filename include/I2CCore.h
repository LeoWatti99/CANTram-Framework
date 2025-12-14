#ifndef I2C_CORE_H
#define I2C_CORE_H

#include "HardwareResource.h"
/*
Abstract class to define a generic interface to use I2C communication*/
class I2CCore : public HardwareResource {
  public:
    virtual bool reset(){ 
      _i2cStatus = I2C_NOT_STARTED;
      return HardwareResource::reset();
    };

    enum I2CStatus {
        I2C_NOT_STARTED=0,
        I2C_STARTED=1
    };

    //sets hardware parameters
    virtual bool begin() =0;
    
    virtual HardwareResource::Type getType() override { return Type::I2C; }
    
    virtual void setPins(int8_t sdaPin, int8_t sclPin) { _sdaPin = sdaPin; _sclPin = sclPin; };
    virtual void setBufferSize(uint32_t bufferSize) { _bufferSize = bufferSize; };
    virtual void setI2CNum(int8_t i2c_num) { _i2c_num = i2c_num; };
    virtual void setFrequency(uint32_t frequency) { _frequency = frequency; };
    virtual uint32_t setMaxFrequency(uint32_t frequency) {
      if(frequency < _frequency) {
        _frequency = frequency;
        INFO_PRINTLN("[I2CCore] Max allowed I2C frequency set to " + String(_frequency) + " Hz");
      }
      return _frequency;
    }
    virtual void beginTransmission(uint8_t address)=0;
    virtual uint8_t endTransmission()=0;
    virtual size_t write(uint8_t data)=0;
    virtual size_t write(const uint8_t *data, size_t quantity)=0;
    virtual size_t requestFrom(uint8_t address, size_t len, bool stopBit)=0;
    virtual int available()=0;
    virtual int read()=0;
    virtual int peek(){ return -1; }; // Optional override to peek at next byte in receive buffer
    virtual void flush() = 0;
    virtual uint8_t getMaxUsages() override { return 0; } //I2C bus can be shared infinitely
    virtual I2CStatus getI2CStatus() const { return _i2cStatus; }
    static constexpr uint32_t I2C_DEFAULT_FREQUENCY = 1700000; // 1,7 MHz default frequency
  protected:
    // Protected members here
    static constexpr uint16_t I2C_DEFAULT_BUFFER_LENGTH = 32;
    
    int8_t _sdaPin=-1;
    int8_t _sclPin=-1;
    uint32_t _frequency= I2C_DEFAULT_FREQUENCY;
    int8_t _i2c_num=-1;
    uint32_t _bufferSize= I2C_DEFAULT_BUFFER_LENGTH;
    I2CStatus _i2cStatus = I2C_NOT_STARTED;
    
};

#endif