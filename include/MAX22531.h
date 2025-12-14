/**
 * @file MAX22531.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MAX22531_H
#define MAX22531_H

#include <Arduino.h>
#include <SPI.h>
#include "MAX22531_registers.h"
#include "SPIChip.h"
#include "Debug.h"


class MAX22531 : public SPIChip
{
    public:
    struct BurstResponse{
        uint8_t status;
        uint16_t adc1;
        uint16_t adc2;
        uint16_t adc3;
        uint16_t adc4;
        uint16_t interrupt;
        uint8_t crc;
        bool filtered;
    };

    enum ERROR_CODES{
        STATUS_OK = 0,
        INVALID_CHANNEL = 10,
        INVALID_REGISTER = 20,
        NULL_POINTER = 30,
        
    };

        MAX22531(uint8_t csPin, uint8_t intPin);
        MAX22531() = default;
        ~MAX22531() = default;
        bool begin();
        uint8_t writeRegister(uint8_t address, uint16_t data);
        uint16_t readRegister(uint8_t address, uint16_t* data = nullptr);
        BurstResponse burstRead(bool filtered = true);
        uint16_t readChannel(uint8_t channel, uint16_t* data = nullptr, bool filtered = true);
        BurstResponse readAllChannels();
        uint8_t HWreset();  

    private:
    
    SPISettings _spiSettings = SPISettings(10000000, MSBFIRST, SPI_MODE0);
    uint8_t resetPORFlag();
    int8_t _intPin = -1;
    bool useInt;
    uint8_t calculateCRC(uint8_t *data, uint8_t length);
    static constexpr uint8_t MAX_START_ATTEMPTS = 5;
    static constexpr uint16_t START_RETRY_DELAY_MS = 1000;

};

#endif