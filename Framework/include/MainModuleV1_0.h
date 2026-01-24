/**
 * @file MainModuleV1_0.h
 * @author your name
 * @brief 
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MAINMODULEV1_0_H
#define MAINMODULEV1_0_H

#include <Arduino.h>
#include "CANTramModule.h"
#include "CANTramCore.h"
#include <PWMCore.h>
#include "ESP32PWMCore.h"
#include "OutputDefinition.h"
#include "Debug.h"
#include "ESP32_UART.h"
#include "UARTInterface.h"
#include "HardwareResource.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"
#include "esp_adc_cal.h"
#include "Adafruit_MCP23X17.h"
#include "ESP32_CANCore.h"
#include "CANInterface.h"
#include "ESP32_I2CCore.h"

class MainModuleV1_0 : public CANTramModule, public PWMOutputProvider, public UARTProvider {
    public:
    MainModuleV1_0();
    static const String HW_TYPE;
    String getHWType() const override { return HW_TYPE; }
    static const String HW_VERSION;
    String getHWVersion() const override { return HW_VERSION; }
    static const String FW_VERSION;
    String getFWVersion() const override { return FW_VERSION; }
    static constexpr uint8_t GPIO_DEMAND = 0;
    uint8_t getGPIODemand() const override { return GPIO_DEMAND; }
    static constexpr uint8_t GPIO_SUPPLY = 25;
    uint8_t getGPIOSupply() const override { return GPIO_SUPPLY; }

    // OutputProvider interface
    bool configureOutput(OutputDefinition* def, uint8_t mode=OUTPUT) override;
    bool setOutput(OutputDefinition* def, bool state) override;
    bool getOutput(OutputDefinition* def) override;

    //Shift register Pins
    Adafruit_MCP23X17 shiftRegister;
    //Chip IC7
    static const uint8_t SR_PIN_0 = 0;
    static const uint8_t SR_PIN_1 = 1;
    static const uint8_t SR_PIN_2 = 2;
    static const uint8_t SR_PIN_3 = 3;
    static const uint8_t SR_PIN_4 = 4;
    static const uint8_t SR_PIN_5 = 5;
    static const uint8_t SR_PIN_6 = 6;
    static const uint8_t SR_PIN_7 = 7;
    static const uint8_t SR_PIN_8 = 8;
    static const uint8_t SR_PIN_9 = 9;
    static const uint8_t SR_PIN_10 = 10;
    static const uint8_t SR_PIN_11 = 11;
    static const uint8_t SR_PIN_12 = 12;
    static const uint8_t SR_PIN_13 = 13;
    static const uint8_t SR_PIN_14 = 14;
    static const uint8_t SR_PIN_15 = 15;

    OutputDefinition EXTEND[GPIO_SUPPLY] = {
        // isShift, pinOrBit, adc, pwm, inputOnly, this, LOW
        OutputDefinition(false, UART_RX, false, true, false, this),   // 0
        OutputDefinition(false, UART_TX, false, true, false, this),   // 1
        OutputDefinition(false, GPIO_NUM_32, false, true, false, this),   // 2
        OutputDefinition(false, GPIO_NUM_15, false, true, false, this),   // 3
        OutputDefinition(false, GPIO_NUM_33, false, true, false, this),    // 4
        OutputDefinition(false, GPIO_NUM_2, false, true, false, this),   // 5
        OutputDefinition(false, GPIO_NUM_25, false, true, false, this),    // 6
        OutputDefinition(false, GPIO_NUM_0, false, true, false, this),   // 7
        OutputDefinition(false, GPIO_NUM_26, false, true, false, this),    // 8
        OutputDefinition(false, GPIO_NUM_4, false, true, false, this),   // 9
        OutputDefinition(false, GPIO_NUM_27, false, true, false, this),    // 10
        OutputDefinition(false, GPIO_NUM_5, false, true, false, this),   // 11
        OutputDefinition(true, SR_PIN_15, false, false, false, this),   // 12
        OutputDefinition(true, SR_PIN_2, false, false, false, this),      // 13
        OutputDefinition(true, SR_PIN_14, false, false, false, this),       // 14
        OutputDefinition(true, SR_PIN_3, false, false, false, this),      // 15
        OutputDefinition(true, SR_PIN_13, false, false, false, this),       // 16
        OutputDefinition(true, SR_PIN_4, false, false, false, this),      // 17
        OutputDefinition(true, SR_PIN_12, false, false, false, this),       // 18
        OutputDefinition(true, SR_PIN_5, false, false, false, this),      // 19
        OutputDefinition(true, SR_PIN_11, false, false, false, this),       // 20
        OutputDefinition(true, SR_PIN_6, false, false, false, this),      // 21
        OutputDefinition(true, SR_PIN_10, false, false, false, this),       // 22
        OutputDefinition(true, SR_PIN_7, false, false, false, this),       // 23
        OutputDefinition(true, SR_PIN_9, false, false, false, this)       // 24
    };

// Temperatur Pin
static const uint8_t TEMP_PIN = GPIO_NUM_35; //using ADC1 on ESP32
static const adc1_channel_t TEMP_ADC_CHANNEL = ADC1_GPIO35_CHANNEL; // GPIO35 is ADC1_CHANNEL_7
static constexpr uint16_t TEMP_RES_1 = 6200; // Resistance at 25 degrees Celsius
#ifdef MAINMODULE_TEMPERATURE_PROTOTYPE_BOARD  //Use This buildflag, if you are using the temperaturesensor prototype board for testing
#warning "Using temperature prototype board settings. This will result in wrong temperatures if you are using the mainmodule board."
#warning "Make sure to clear the buildflag MAINMODULE_TEMPERATURE_PROTOTYPE_BOARD buildflag for production builds."
static constexpr uint16_t TEMP_RES_2 = 12000; // Resistance at 25 degrees Celsius
#else
static constexpr uint16_t TEMP_RES_2 = 6200; // Resistance at 25 degrees Celsius
#endif
static constexpr float TEMP_FAKTOR = float(TEMP_RES_2)/float(TEMP_RES_1 + TEMP_RES_2); //Faktor for voltage divider
static constexpr float TEMP_SUPPLY_VOLTAGE = 5000.0; // in millivolts
static constexpr uint8_t TEMP_MULTISAMPLE = 16; //Number of samples to average
static constexpr uint16_t ADC_MAX_VOLTAGE = 2450; //in millivolts
esp_adc_cal_characteristics_t adc1_chars;
static constexpr uint16_t ADC_DEFAULT_VREF = 1100;
bool adc_calibrated;


// CAN Pins
ESP32_CANCore canCore;
static const uint8_t CAN_TX = GPIO_NUM_17;
static const uint8_t CAN_RX = GPIO_NUM_16;
CANInterface canInterface;

// SPI Pins
static const uint8_t SPI_COPI = GPIO_NUM_21;
static const uint8_t SPI_CIPO = GPIO_NUM_18;
static const uint8_t SPI_SCK = GPIO_NUM_22;

// I2C Pins
static const uint8_t I2C_SDA = GPIO_NUM_13;
static const uint8_t I2C_SCL = GPIO_NUM_14;

// UART Pins
static const uint8_t UART_TX = GPIO_NUM_19;
static const uint8_t UART_RX = GPIO_NUM_23;

// INT Pin
static const uint8_t INT_PIN = GPIO_NUM_34;

//Enable Pin 
static const uint8_t GENERAL_EN = SR_PIN_8; 

//LEDs
static const uint8_t LED_WARNING = SR_PIN_0;
static const uint8_t LED_ERROR = SR_PIN_1;

//public funtions
        bool reset()override;
        bool attachModule(uint8_t slot,uint8_t gpioStart) override;
        
        bool provideGPIOs() override;
        bool requestGPIOs() override; 
        bool provideHardwareResources() override; 
        bool requestHardwareResources() override;
        bool addInterfaces() override;
        bool preInitialize() override;
        bool initialize() override;

        void enableOutputs(bool state) override;

        uint16_t getTemperatureCelsius();

       
        void cycle(uint8_t* response) override;

         Interface** getInterfaces() override { return _interfaces; }
        size_t getInterfaceCount() override { return INTERFACE_COUNT; }

        
        bool setPWM(OutputDefinition* def, uint16_t value) override;
        bool registerPWMChannel(OutputDefinition* def) override;
    private:
        static constexpr size_t INTERFACE_COUNT = 1; // Number of interfaces in the array
        Interface* _interfaces[INTERFACE_COUNT]; // representation of the physical interfaces clamps
        ESP32PWMCore _pwmCore;
        ESP32_UART uart;
        ESP32_I2CCore i2cCore;

        static constexpr uint8_t UART_NUM = UART_NUM_1;
        static constexpr uint16_t UART_BUFFER_SIZE = 1024;

        

        bool startShiftRegister();
        bool resetShiftRegister();
        
        enum ShiftRegistertStatus{
            NOT_STARTED,
            STARTED,
            FAILED
        };

        static constexpr uint8_t SHIFTREGISTER_I2C_ADDRESS = 0b100111;;
        static constexpr uint32_t SHIFTREGISTER_I2C_FREQUENCY = 400000; //400 kHz allowed frequency for MCP23X17
        ShiftRegistertStatus _shiftregisterStatus = NOT_STARTED;

    protected:
        virtual UARTCore* getUartCore() override { return &uart; }
        
};  


#endif

