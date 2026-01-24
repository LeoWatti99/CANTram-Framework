#ifndef ANALOG_MODULE_V1_0_H
#define ANALOG_MODULE_V1_0_H

#include <Arduino.h>
#include "CANTramModule.h"
#include "MAX22531.h"
#include "Interface.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "OutputDefinition.h"
#include "Debug.h"

class AnalogModuleV1_0 : public CANTramModule {
    public:

    AnalogModuleV1_0() =default;

    //Module information
    static const String HW_TYPE;
    String getHWType() const override { return HW_TYPE; }
    static const String HW_VERSION;
    String getHWVersion() const override { return HW_VERSION; }
    static const String FW_VERSION;
    String getFWVersion() const override { return FW_VERSION; }
    static constexpr uint8_t GPIO_DEMAND = 5;
    uint8_t getGPIODemand() const override { return GPIO_DEMAND; }
    static constexpr uint8_t GPIO_SUPPLY = 0;
    uint8_t getGPIOSupply() const override { return GPIO_SUPPLY; }

    //Core functions
    bool attachModule(uint8_t slot, uint8_t gpioStart) override;
    bool provideGPIOs() override;
    bool requestGPIOs() override;
    bool provideHardwareResources() override;
    bool requestHardwareResources() override;
    bool addInterfaces() override;
    bool initialize() override;
    void cycle(uint8_t* response) override;
    bool reset() override;

    //Interfaces
    Interface** getInterfaces() override { return _interfaces; }
    size_t getInterfaceCount() override { return INTERFACE_COUNT; }

    private:
        static constexpr size_t INTERFACE_COUNT = 8;
        Interface* _interfaces[INTERFACE_COUNT];
        AnalogInput _analogInputInterfaces[4];
        AnalogOutput _analogOutputInterfaces[4];
        OutputDefinition* CS_INPUTS; //Chip select pin for the input chip
        OutputDefinition* PWM_OUTPUTS[4]; //Pins for analog outputs

        //Hardware
        MAX22531 _Inputs;
        Interface::Resolution _resolution = Interface::RES_12BIT;
};

#endif