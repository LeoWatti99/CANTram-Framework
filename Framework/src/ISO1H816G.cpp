#include "ISO1H816G.h"
#include <SPI.h>
#include "Debug.h"


ISO1H816G::ISO1H816G(uint8_t csPin, uint8_t disPin, uint8_t diagPin) : SPIChip(csPin), _disPin(disPin), _diagPin(diagPin)
{
    
    if (disPin != -1){
    pinMode(_disPin, OUTPUT);
    digitalWrite(_disPin, LOW); // Set DIS low
    useDisable = true;
    } 

    if(diagPin != -1){
    pinMode(_diagPin, INPUT);
    useDiag = true;
    }
    
    
}


void ISO1H816G::setPins(uint8_t csPin, uint8_t disPin, uint8_t diagPin){

    SPIChip::setCSPin(csPin);
    INFO_PRINTLN("[ISO1H816G] INFO: CS pin set to "+ String(_csPin));

    if (disPin != -1){
    _disPin = disPin;
    pinMode(_disPin, OUTPUT);
    digitalWrite(_disPin, LOW); // Set DIS low
    useDisable = true;
    INFO_PRINTLN("[ISO1H816G] INFO: DIS pin set to "+ String(_disPin));
    } else {
        useDisable = false;
    }
    

    if(diagPin != -1){
    _diagPin = diagPin;
    pinMode(_diagPin, INPUT);
    useDiag = true;
    INFO_PRINTLN("[ISO1H816G] INFO: DIAG pin set to "+ String(_diagPin));
    } else {
        useDiag = false;
    }
}

uint8_t ISO1H816G::begin()
{
    if (!SPI.bus()) {
        ERROR_PRINTLN("[ISO1H816G] SPI wasn't initialized. Please call SPI.begin() first.");
        return SPI_FAULT;
    }
    SPIChip::deselectChip();
    _initialized=true;
    INFO_PRINTLN("[ISO1H816G] Initialized");
    return STATUS_OK;

}

uint8_t ISO1H816G::enable()
{
    if(!_initialized) return NOT_INITIALIZED;
    if(!useDisable) return DISABLE_PIN_NOT_SET; // Already enabled if no disable pin used

    digitalWrite(_disPin, HIGH); // Set DIS high to enable the device
    _enabled = true;
    return STATUS_OK;
}

void ISO1H816G::disable(){
    if(!useDisable) return; // Already disabled if no disable pin used
    digitalWrite(_disPin, LOW); // Set DIS low to disable the device
    _enabled = false;
}

uint8_t ISO1H816G::setOutputs(uint8_t outputs)
{
    if(!_initialized)// Check if the device is enabled
    {
        ERROR_PRINTLN("[ISO1H816G] Device not initialized. Call begin() first.");
     return NOT_INITIALIZED; 
    }

    _outputs = outputs;
    SPI.beginTransaction(_spiSettings);

    //handle chip select
    SPIChip::selectChip();  
        SPI.transfer(outputs); // Write the outputs to the device

    //handle chip select
    SPIChip::deselectChip();
    SPI.endTransaction();
    DEBUG_PRINTLN("[ISO1H816G] Outputs set to: " + String(outputs, BIN));

    return STATUS_OK;
}

uint8_t ISO1H816G::setOutput(uint8_t output, bool state)
{
    if(!_initialized) return NOT_INITIALIZED; // Check if the device is enabled

    if (output > 7) return INVALID_CHANNEL; // Invalid output

    if (state) {
        _outputs |= (1 << output); // Set the bit for the output
    } else {
        _outputs &= ~(1 << output); // Clear the bit for the output
    }

    setOutputs(_outputs);   

    return STATUS_OK;
}

void ISO1H816G::OnFault(void (*callback)()){
    if(!useDiag){
        ERROR_PRINTLN("[ISO1H816G] ERROR: No DIAG pin set. Cannot attach interrupt.");
        return; // No diag pin set
    } 
    attachInterrupt(_diagPin, callback, FALLING);
}

