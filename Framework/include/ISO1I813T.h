#ifndef ISO1I813T_H
#define ISO1I813T_H
#include <Arduino.h>
#include "ISO1I813T_registers.h"
#include <SPI.h>
#include "SPIChip.h"
#include "Debug.h"



class ISO1I813T : public SPIChip
{
    public:
    enum ISO1I813T_Filter{
        US_50,
        US_100,
        US_400,
        US_800,
        US_1600,
        US_3200,
        US_10000,
        US_20000,
        FILTER_OFF
    }; 
    
    enum ERROR_CODES{
        STATUS_OK = 0,
        INVALID_CHANNEL = 10,
        INVALID_FILTER = 11,
        WIRE_BREAK_SETTING_ERROR = 30
        
    };
    
    struct Diag
    {
        uint8_t glErr: 8; // GL error
        uint8_t intErr: 8; // Interrupt error
    };
    
    struct Data
    {
        uint8_t inputs: 8; // Inputs
        uint8_t wireBreaks: 8; // Wire breaks
    };
        uint8_t daisyChain = 1; //number of devices in daisy chain, default 1, zero means no daisy chain support
        ISO1I813T()=default;
        ISO1I813T(uint8_t csPin, uint8_t syncPin);
        void writeRegister(uint8_t address,uint8_t data);
        uint8_t readRegister(uint8_t address);
        void setPins(uint8_t csPin, uint8_t syncPin=-1);
        void begin();

        uint8_t getInputs() { return _inputs; }
        bool getInput(uint8_t input) { return (_inputs & (1 << input)) != 0; }

        Data fetchData();
        Diag fetchDiagnose();
        uint8_t getWireBreaks() { return _wireBreaks; }
        bool getWireBreak(uint8_t wireBreak) { return (_wireBreaks & (1 << wireBreak)) != 0; }

        uint8_t setFilter(uint8_t channel, ISO1I813T_Filter filter);
        uint8_t setWireBreakMask(uint8_t channel, bool enable);

        void freeze(bool status) {digitalWrite(_syncPin, (status && useSync) ? LOW : HIGH);}
        
        
        
    private:
        uint8_t _syncPin;
        SPISettings _spiSettings = SPISettings(5000000, MSBFIRST, SPI_MODE0);
        void _setSPISettings(uint32_t speed,uint8_t bitOrder, uint8_t dataMode);
        uint8_t _inputs;
        uint8_t _wireBreaks;
        bool useSync = false;
};


#endif