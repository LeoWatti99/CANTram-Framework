#include "ISO1I813T.h"
#include <SPI.h>
#include "Debug.h"

ISO1I813T::ISO1I813T(uint8_t csPin, uint8_t syncPin) : SPIChip(csPin), _syncPin(syncPin), _inputs(0), _wireBreaks(0)
{
    if(_syncPin != -1){
    pinMode(_syncPin, OUTPUT);
    digitalWrite(_syncPin, HIGH); // Set SYNC low to enable the device
    useSync = true;
    }
}

void ISO1I813T::_setSPISettings(uint32_t speed,uint8_t bitOrder, uint8_t dataMode)
{
    _spiSettings = SPISettings(speed, bitOrder, dataMode);
}

void ISO1I813T::writeRegister(uint8_t address,uint8_t data)
{
    CHIP_PRINTLN("[ISO1I813T] Writing 0x" + String(data, HEX) + " to register 0x" + String(address, HEX));
    
    auto spiWrite = [&](uint8_t address, uint8_t data) {
        SPI.beginTransaction(_spiSettings);

        SPIChip::selectChip();
        _inputs |= SPI.transfer((1 << ISO1I813T_WRITE_BIT) | address) << 8; // Write bit and address
        CHIP_PRINTLN("[ISO1I813T] Inputs: 0b" + String(_inputs, BIN));
        _wireBreaks |= SPI.transfer(data); // Data to write
        CHIP_PRINTLN("[ISO1I813T] Written data: 0x" + String(data, HEX)+ " to register 0x" + String(address, HEX));
        CHIP_PRINTLN("[ISO1I813T] Wire breaks: 0b" + String(_wireBreaks, BIN));
        SPIChip::deselectChip();
        SPI.endTransaction();
    };

    CHIP_PRINTLN("[ISO1I813T] Daisy chain size: " + String(daisyChain));
    for (int i = 0; i < daisyChain; i++) {
        spiWrite(address, data);
    }
    
}

uint8_t ISO1I813T::readRegister(uint8_t address)
{
    CHIP_PRINTLN("[ISO1I813T] Reading register 0x" + String(address, HEX));
    uint8_t data= 0;
    auto spiRead = [&](uint8_t address) {
        uint8_t result = 0;
        SPI.beginTransaction(_spiSettings);
        SPIChip::selectChip();

        // High byte is always the inputs
        
        _inputs = SPI.transfer(address & ~(1 << ISO1I813T_WRITE_BIT)); // Address with read bit, write high byte
        CHIP_PRINTLN("[ISO1I813T] Inputs: 0b" + String(_inputs, BIN));
        // Low byte is always the requested data
        result = SPI.transfer(0x00); // Dummy byte to read data, write low byte
        CHIP_PRINTLN("[ISO1I813T] Read data: 0x" + String(result, HEX)+ " in register 0x" + String(address, HEX));
         _wireBreaks |= result; // Store wire break status
        SPIChip::deselectChip();
        SPI.endTransaction();
        return result;
    };
    CHIP_PRINTLN("[ISO1I813T] Daisy chain size: " + String(daisyChain));
    for(int i = 0; i < daisyChain; i++){
        data = spiRead(address);
    }
    CHIP_PRINTLN("[ISO1I813T] Final read data: 0x" + String(data, HEX)+ " in register 0x" + String(address, HEX));
    return data;
}

void ISO1I813T::setPins(uint8_t csPin, uint8_t syncPin){
    SPIChip::setCSPin(csPin);

    if (syncPin != -1){
    _syncPin = syncPin;
    pinMode(_syncPin, OUTPUT);
    digitalWrite(_syncPin, HIGH); // Set SYNC low to enable the device
    useSync = true;
    } else {
        useSync = false;
    }
}

void ISO1I813T::begin()
{
    if (!SPI.bus()) {
        Serial.println("[ISO1I813T] SPI wasn't initialized. Please call SPI.begin() first.");
        return;
        
    }
    SPIChip::deselectChip();
    fetchData();
    fetchDiagnose();
    
}

uint8_t ISO1I813T::setFilter(uint8_t channel, ISO1I813T_Filter filter)
{
    if(channel > 7) return INVALID_CHANNEL; // Invalid channel
    if(filter > FILTER_OFF) return INVALID_FILTER; // Invalid filter
    
    writeRegister(ISO1I813T_REG_COEFIL0_WRITE + (channel * 2), filter);
    return STATUS_OK;
}

uint8_t ISO1I813T::setWireBreakMask(uint8_t channel, bool enable)
{
    if(channel > 7) return INVALID_CHANNEL; // Invalid channel

    writeRegister(ISO1I813T_REG_COEFIL0_WRITE + (channel * 2), (enable<< ISO1I813T_WB_BIT)); // Set wire break mask
    uint8_t temp = readRegister(ISO1I813T_REG_COEFIL0_READ + (channel * 2));
    if(((temp >> ISO1I813T_WB_BIT) & 0x01) != enable) return WIRE_BREAK_SETTING_ERROR; // Error setting wire break mask
    return STATUS_OK;
}

ISO1I813T::Data ISO1I813T::fetchData() //Outputs the inputs in the high byte and the wire breaks in the low byte
{
    Data returnData;
    returnData.wireBreaks = readRegister(ISO1I813T_REG_DIAG);
    returnData.inputs = _inputs;
    return returnData;
}

ISO1I813T::Diag ISO1I813T::fetchDiagnose() //Outputs the error registers of the device
{
    Diag returnData;
    returnData.glErr = readRegister(ISO1I813T_REG_GLERR);
    returnData.intErr = readRegister(ISO1I813T_REG_INTERR);
    return returnData;
}