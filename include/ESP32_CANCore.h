#ifndef ESP32_CANCORE_H
#define ESP32_CANCORE_H

#include "CANCore.h"
/**
 * @file ESP32_CANCore.h
 * @brief Declaration of the ESP32_CANCore class.
 * @details Defines the ESP32_CANCore class which implements CANCore functionalities for the ESP32 platform using the TWAI driver.
 */
class ESP32_CANCore : public CANCore {
public:
    ESP32_CANCore()=default;
    ~ESP32_CANCore()=default;

    HardwareResource::Type getType() override { return HardwareResource::CAN; }
    bool begin() override;
    bool setBaudrate(Baudrate baudrate) override;
    bool setPins(int8_t txPin, int8_t rxPin) override;
    bool end() override;
    bool sendMessage(const CANMessage& message) override;
    bool readMessage(CANMessage& message) override;
    uint8_t available() override;
    bool setupFilter(uint32_t id, uint32_t mask) override;
    bool reset() override;

private:
    
};

#endif