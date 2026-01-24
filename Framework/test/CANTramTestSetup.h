/*
@file CANTramTestSetup.h
@brief Collection of slot and gpio start definitions for generic test setup.
This setup constists of:
- MainModuleV1_0 in slot 0
- BusModuleV1_0 in slot 1
- DigitalModuleV1_0 in slot 2
- AnalogModuleV1_0 in slot 3
- RelaisModuleV1_0 in slot 4
Each module is assigned a slot and a GPIO start based on the GPIO demand of the previous modules.

*/

#ifndef CAN_TRAM_TEST_SETUP_H
#define CAN_TRAM_TEST_SETUP_H

#include <SPI.h>
#include "Debug.h"

#define MAIN_MODULE_SLOT 0
#define BUS_MODULE_SLOT 1
#define ANALOG_MODULE_SLOT 2
#define DIGITAL_MODULE_SLOT 3
#define RELAIS_MODULE_SLOT 4

#define MAINMODULE_GPIO_DEMAND 0
#define BUSMODULE_GPIO_DEMAND 3
#define DIGITALMODULE_GPIO_DEMAND 2
#define ANALOGMODULE_GPIO_DEMAND 5
#define RELAISMODULE_GPIO_DEMAND 4

#define MAINMODULE_GPIO_START 0
#define BUSMODULE_GPIO_START (MAINMODULE_GPIO_START + MAINMODULE_GPIO_DEMAND)
#define ANALOGMODULE_GPIO_START (BUSMODULE_GPIO_START + BUSMODULE_GPIO_DEMAND)
#define DIGITALMODULE_GPIO_START (ANALOGMODULE_GPIO_START + ANALOGMODULE_GPIO_DEMAND)
#define RELAISMODULE_GPIO_START (DIGITALMODULE_GPIO_START + DIGITALMODULE_GPIO_DEMAND)

#define ANALOGMODULE_AQ0_DEF (ANALOGMODULE_GPIO_START + 0)
#define ANALOGMODULE_AQ1_DEF (ANALOGMODULE_GPIO_START + 1)
#define ANALOGMODULE_AQ2_DEF (ANALOGMODULE_GPIO_START + 2)
#define ANALOGMODULE_AQ3_DEF (ANALOGMODULE_GPIO_START + 3)
#define ANALOGMODULE_AI_CS_PIN GPIO_NUM_0
#define ANALOGMODULE_AI_CS_DEF (ANALOGMODULE_GPIO_START + 4)

#define BUSMODULE_SPI_CS_PIN GPIO_NUM_32
#define BUSMODULE_SPI_CS_DEF (BUSMODULE_GPIO_START + 2)

#define DIGITALMODULE_DQ_CS_PIN GPIO_NUM_26
#define DIGITALMODULE_DQ_CS_DEF (DIGITALMODULE_GPIO_START + 0)
#define DIGITALMODULE_DI_CS_PIN GPIO_NUM_4
#define DIGITALMODULE_DI_CS_DEF (DIGITALMODULE_GPIO_START + 1)



#define SPI_COPI_PIN GPIO_NUM_21
#define SPI_CIPO_PIN GPIO_NUM_18
#define SPI_SCK_PIN GPIO_NUM_22

// Chip select pins for devices
#define SPI_CS_DI GPIO_NUM_4
#define SPI_CS_DQ GPIO_NUM_26
#define SPI_CS_AI GPIO_NUM_0

/**
 * @brief Setup the SPI test environment.  
 * @details This function initializes the SPI bus and configures the chip select pins for the test environment.
 * @note This if no CANTramCore is used, the SPI bus is initialized directly. If CANTramCore is used, it assumes a MainModuleV1_0 is attached and configures the CS pins via the core.
 * 
 */
void setupSPITestEnvironment()
{
    #ifndef CANTRAMCORE_H
    // Initialize SPI
    DEBUG_PRINTLN("Setting up SPI for test environment without CANTramCore...");
    SPI.begin(SPI_SCK_PIN, SPI_CIPO_PIN, SPI_COPI_PIN, -1); // SCK, MISO, MOSI, SS
    pinMode(SPI_CS_DI, OUTPUT);
    pinMode(SPI_CS_DQ, OUTPUT);
    pinMode(SPI_CS_AI, OUTPUT);
    digitalWrite(SPI_CS_DI, HIGH);
    digitalWrite(SPI_CS_DQ, HIGH);
    digitalWrite(SPI_CS_AI, HIGH);
    delay(500);
    #else
    if(CANTramCore::getModule(0) == nullptr) {
        CRITICAL_ERROR_PRINTLN("No MainModuleV1_0 attached to CANTramCore,  Pls attach MainModuleV1_0 to use CANTramCore SPI setup.");
    }
    DEBUG_PRINTLN("Setting up SPI for test environment with CANTramCore...");
    
    OutputDefinition* spiCSDef = CANTramCore::getOutputDefinition(BUSMODULE_SPI_CS_DEF);
    spiCSDef->provider->configureOutput(spiCSDef, OUTPUT);
    spiCSDef->initialValue = HIGH;

    OutputDefinition* dqCSDef = CANTramCore::getOutputDefinition(DIGITALMODULE_DQ_CS_DEF);
    dqCSDef->provider->configureOutput(dqCSDef, OUTPUT);
    dqCSDef->initialValue = HIGH;

    OutputDefinition* diCSDef = CANTramCore::getOutputDefinition(DIGITALMODULE_DI_CS_DEF);
    diCSDef->provider->configureOutput(diCSDef, OUTPUT);
    diCSDef->initialValue = HIGH;

    OutputDefinition* aiCSDef = CANTramCore::getOutputDefinition(ANALOGMODULE_AI_CS_DEF);
    aiCSDef->provider->configureOutput(aiCSDef, OUTPUT);
    aiCSDef->initialValue = HIGH;
    #endif

}

template <int16_t SAMPLE_COUNT>
class MeasurementArray{
    public:
        MeasurementArray()=default;
            
            
            uint64_t getAverage(){
                uint64_t sum =0;
                for(uint16_t i=0;i<index;i++){
                    sum += sample[i];
                }
                return sum / index;
            }
            uint64_t getMax(){
                uint16_t maxVal =0;
                for(uint16_t i=0;i<index;i++){
                    if(sample[i] > maxVal){
                        maxVal = sample[i];
                    }
                }
                return maxVal;
            }
            uint64_t getMin(){
                uint64_t minVal =4095;
                for(uint16_t i=0;i<index;i++){
                    if(sample[i] < minVal){
                        minVal = sample[i];
                    }
                }
                return minVal;
            }
            void addSample(uint64_t value){
                if(index >= SAMPLE_COUNT){
                    ERROR_PRINTLN("Multisample overflow");
                    return;
                }
                    sample[index++] = value;  
            }
    private:
        const uint16_t _count=SAMPLE_COUNT;
        uint64_t sample[SAMPLE_COUNT];
        uint16_t index =0;
};

#endif //CAN_TRAM_TEST_SETUP_H