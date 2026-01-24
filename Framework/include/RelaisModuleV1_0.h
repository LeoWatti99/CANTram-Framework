#ifndef RELAISMODULEV1_0_H
#define RELAISMODULEV1_0_H

#include "CANTramModule.h"
#include <Arduino.h>
#include "Interface.h"
#include "RelaisInterface.h"
#include "Outputdefinition.h"

class RelaisModuleV1_0 : public CANTramModule{
  public:

    RelaisModuleV1_0() = default;
    static const String HW_TYPE;
    String getHWType() const override { return HW_TYPE; }
    static const String HW_VERSION;
    String getHWVersion() const override { return HW_VERSION; }
    static const String FW_VERSION;
    String getFWVersion() const override { return FW_VERSION; }

    static constexpr uint8_t GPIO_DEMAND = 4;
    uint8_t getGPIODemand() const override { return GPIO_DEMAND; }
    static constexpr uint8_t GPIO_SUPPLY = 0;
    uint8_t getGPIOSupply() const override { return GPIO_SUPPLY; }

    bool attachModule(uint8_t slot, uint8_t gpioStart) override;

    bool provideGPIOs() override;
    bool requestGPIOs() override ;
    bool provideHardwareResources() override;
    bool requestHardwareResources() override;
    bool addInterfaces() override;

    bool initialize() override;

    void cycle(uint8_t* response) override;

    Interface** getInterfaces() override;
    size_t getInterfaceCount() override { return INTERFACE_COUNT; }

  private:
    
    static constexpr size_t INTERFACE_COUNT = 4;
    RelaisInterface _relaisInterfaces[INTERFACE_COUNT];
    Interface* _interfaces[INTERFACE_COUNT];
    OutputDefinition* _relaisGPIOs[INTERFACE_COUNT];
    
};

#endif