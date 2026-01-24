#ifndef CANCORE_H
#define CANCORE_H

#include <Arduino.h>
#include "HardwareResource.h"

#include "Debug.h"
/**
 * @file CANCore.h
 * @brief Declaration of the CANCore class.
 * @details Defines the CANCore class which provides core functionalities for CAN bus operations in the CANTram system.
 */
class CANCore : public HardwareResource{ 
public:

struct CANMessage {
    uint32_t    id;          // 11-bit or 29-bit identifier
    bool        isExtended; // True if extended ID (29-bit), false for standard ID (11-bit)
    bool        isRemote;   // True if remote frame
    uint8_t     data[8];    // Data payload (0-8 bytes)
    uint8_t     length;     // Length of data in bytes
    bool        error;      // Flag for error frames  
    String toString() const {
        String msg = "CANMessage[ID: " + String(id) + (isExtended ? " (Extended)" : " (Standard)") + ", Remote: " + String(isRemote) + ", Length: " + String(length) + ", Data: ";
        for(uint8_t i=0; i<length; i++) {
            msg += String(data[i], HEX);
            if(i < length - 1) msg += " ";
        }
        msg += "]";
        return msg;
    }  
};

enum Baudrate {
    BR_NOT_SET = 0,
    BR_125K = 125000,
    BR_250K = 250000,
    BR_500K = 500000,
    BR_1M   = 1000000
};

enum Status{
    STATUS_OK,
    STATUS_ERROR_ACTIVE,
    STATUS_BUS_OFF,
    STATUS_ERROR_PASSIVE
};

enum FilterType {
    FILTER_ACCEPT_ALL,
    FILTER_MATCH_ID,
    FILTER_MASKED
};

    
    virtual bool begin()=0;
    virtual bool setBaudrate(Baudrate baudrate)=0;
    virtual bool setPins(int8_t txPin, int8_t rxPin)=0;
    virtual bool end()=0;
    virtual bool sendMessage(const CANMessage& message)=0;
    virtual bool readMessage(CANMessage& message)=0;
    virtual uint8_t available()=0;
    virtual bool setupFilter(uint32_t id, uint32_t mask)=0;

    Baudrate getBaudrate() const { return _baudrate; }
    uint8_t getTxPin() const { return _txPin; }
    uint8_t getRxPin() const { return _rxPin; }

    virtual bool reset() override {
        INFO_PRINTLN("[CANCore] Resetting CAN core.");
        
        bool success = end();
        _isInitialized = false;
        _txPin = -1;
        _rxPin = -1;
        _filterId = 0;
        _filterMask = 0;
        _baudrate = BR_NOT_SET;
        _filterType = FILTER_ACCEPT_ALL;
        _status = STATUS_OK;
        success &= HardwareResource::reset();
        return success;
    }
    

    protected:  
    CANCore()=default;
    ~CANCore()=default;

    Baudrate _baudrate = BR_NOT_SET;
    FilterType _filterType = FILTER_ACCEPT_ALL;
    int8_t  _txPin = -1;
    int8_t  _rxPin = -1;
    Status   _status = STATUS_OK;
    bool    _isInitialized = false;
    uint32_t _filterId = 0;
    uint32_t _filterMask = 0;


};

#endif