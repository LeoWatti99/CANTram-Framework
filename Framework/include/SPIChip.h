#ifndef SPICHIP_H
#define SPICHIP_H

#include "Debug.h"

using namespace std;

/* Base class for SPI devices in CANTram Controller. This class makes it possible to use external chip select since the SPI device may be selected via a shift register.*/
class SPIChip {
    public:
        
        void activateExternalChipSelect() {
            useExternalChipSelect = true;
        }
        void setCSControlCallback(std::function<void(bool)> callback) {
            csControl = callback;
            useExternalChipSelect = true;
        }
        void setCSPin(int csPin) {
            useExternalChipSelect = false;
            _csPin = csPin;
            pinMode(_csPin, OUTPUT);
            digitalWrite(_csPin, HIGH); // Set CS high to deselect the device
        }
        
    protected:
        int _csPin;
        bool useExternalChipSelect = false;
        SPIChip() = default;
        SPIChip(uint8_t csPin) : _csPin(csPin) {
            useExternalChipSelect = false;
            pinMode(_csPin, OUTPUT);
            digitalWrite(_csPin, HIGH); // Set CS high to deselect the device
        }
        std::function<void(bool)> csControl = nullptr;
        void selectChip() {
            CHIP_PRINTLN("[SPIChip] Select chip");
            if(!SPIChip::useExternalChipSelect){
                CHIP_PRINTLN("[SPIChip] Using internal chip select");
                digitalWrite(_csPin, LOW); // Set CS low
            }
            else{
                CHIP_PRINTLN("[SPIChip] Using external chip select");
                SPIChip::csControl(LOW);
            }
        }
        void deselectChip() {
            CHIP_PRINTLN("[SPIChip] Deselect chip");
            if(!SPIChip::useExternalChipSelect){
                CHIP_PRINTLN("[SPIChip] Using internal chip select");
                digitalWrite(_csPin, HIGH); // Set CS high
            }
            else{
                CHIP_PRINTLN("[SPIChip] Using external chip select");
                SPIChip::csControl(HIGH);
            }
        }

        
};
#endif