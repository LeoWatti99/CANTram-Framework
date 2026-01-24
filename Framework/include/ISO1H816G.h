#ifndef ISO1H816G_H
#define ISO1H816G_H
#include <Arduino.h>
#include <SPI.h>
#include "SPIChip.h"
#include "Debug.h"

class ISO1H816G : public SPIChip 
{
    public:
    enum ERROR_CODES{
        STATUS_OK = 0,
        NOT_INITIALIZED = 5,
        INVALID_CHANNEL = 10,
        SPI_FAULT = 20,
        DISABLE_PIN_NOT_SET = 30,
        DIAG_PIN_NOT_SET = 40
    };
        ISO1H816G()=default;
        ISO1H816G(uint8_t csPin, uint8_t disPin, uint8_t diagPin);
    

        uint8_t begin();
        uint8_t enable();
        void disable();
        bool isEnabled() {return _enabled; }

        uint8_t setOutputs(uint8_t outputs);
        uint8_t setOutput(uint8_t output, bool state);

        uint8_t getOutputs() { return _outputs; }
        bool getOutput(uint8_t output) { return (_outputs & (1 << output)) != 0; }

        void setSPISettings(uint32_t speed, uint8_t bitOrder, uint8_t dataMode) { _spiSettings = SPISettings(speed, bitOrder, dataMode); }
        
        void OnFault(void (*callback)(void));

        void setPins(uint8_t csPin, uint8_t disPin=-1, uint8_t diagPin=-1);
        

    private:
        uint8_t _outputs; // Outputs
        uint8_t _disPin;
        uint8_t _diagPin;
        uint8_t _enabled;
        uint8_t _initialized;
        uint8_t _fault;
        SPISettings _spiSettings = SPISettings(10000000, MSBFIRST, SPI_MODE3);
        bool useDisable;
        bool useDiag;
        
};
#endif