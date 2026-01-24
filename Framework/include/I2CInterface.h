#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "Interface.h"
#include "I2CCore.h"
#include "Wire.h"

class I2CInterface : public Interface{
public:
    I2CInterface() = default;

    Interface::Type getType() const override {
        return Interface::BUS;
    }

    void setI2CCore(I2CCore* core){i2cCore = core;}
    I2CCore* getI2CCore(){return i2cCore;}

    bool begin(){
        if(!i2cCore->begin()){
            ERROR_PRINTLN("[I2CInterface] ERROR: I2C initialization failed.");
            return false;
        }
        return true;  
    }
    
    void beginTransmission(uint16_t address){
        i2cCore->beginTransmission((uint8_t)address);
    };
    void beginTransmission(uint8_t address){
        i2cCore->beginTransmission(address);
    };

    void beginTransmission(int address){
        i2cCore->beginTransmission((uint8_t)address);
    };

    uint8_t endTransmission(bool sendStop){
        return i2cCore->endTransmission();
    };
    uint8_t endTransmission(void){
        return i2cCore->endTransmission();
    };

    size_t write(uint8_t data){
        return i2cCore->write(data);
    };
    size_t write(const uint8_t* data, size_t length){
        return i2cCore->write(data, length);
    }
    int available(void){
        return i2cCore->available();
    };
    int read(void){
        return i2cCore->read();
    };
    int peek(void){
        return i2cCore->peek();
    };
    void flush(void){
        i2cCore->flush();
    };

    inline size_t write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }

private:
    I2CCore* i2cCore;
};
    #endif