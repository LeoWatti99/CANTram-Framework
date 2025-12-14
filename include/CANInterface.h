#ifndef CANINTERFACE_H
#define CANINTERFACE_H


#include <Arduino.h>
#include "Interface.h"
#include "CANCore.h"
/**
 * @file CANInterface.h
 * @brief Declaration of the CANInterface class.
 * @details Defines the CANInterface class which represents a CAN bus interface in the CANTram system.
 */

class CANInterface : public Interface {
public:

    Interface::Type getType() const override { return Interface::BUS; }
    CANInterface()=default;
    bool begin(){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before begin().");
            return false;
        }
        if(!_canCore->begin()){
            ERROR_PRINTLN("[CANInterface] ERROR: CAN initialization failed.");
            return false;
        }
        return true;  
    }

    bool end(){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before end().");
            return false;
        }
        if(!_canCore->end()){
            ERROR_PRINTLN("[CANInterface] ERROR: CAN deinitialization failed.");
            return false;
        }
        return true;  
    }

    bool setBaudrate(CANCore::Baudrate baudrate){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before setBaudrate().");
            return false;
        }
        if(!_canCore->setBaudrate(baudrate)){
            ERROR_PRINTLN("[CANInterface] ERROR: Failed to set CAN baudrate.");
            return false;
        }
        return true;
    }

    bool sendMessage(const CANCore::CANMessage& message){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before sendMessage().");
            return false;
        }
        if(!_canCore->sendMessage(message)){
            ERROR_PRINTLN("[CANInterface] ERROR: Failed to send CAN message.");
            return false;
        }
        return true;
    }

    bool receiveMessage(CANCore::CANMessage& message){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before receiveMessage().");
            return false;
        }
        return _canCore->readMessage(message);
    }
    uint8_t available(){
        if(!_canCore){
            DEV_ERROR_PRINTLN("[CANInterface] ERROR: No CANCore assigned. Call setCANCore() before available().");
            return 0;
        }
        return _canCore->available();
    }

    void setCANCore(CANCore* canCore) { _canCore = canCore; }

private:
    CANCore* _canCore;

};

#endif // CANINTERFACE_H