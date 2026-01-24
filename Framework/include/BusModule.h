#ifndef BUSModuleV1_0_H
#define BUSModuleV1_0_H
#include "CANTramCore.h"
#include "CANTramModule.h"
#include <SPI.h>
#include <Arduino.h>
#include "SPIChip.h"
#include "Debug.h"
#include "Interface.h"
#include "UARTCore.h"
#include "UARTInterface.h"
#include "I2CCore.h"
#include "I2CInterface.h"

class BusModuleV1_0 : public CANTramModule {
    public:
    static const String HW_TYPE;
    String getHWType() const override { return HW_TYPE; }
    static const String HW_VERSION;
    String getHWVersion() const override { return HW_VERSION; }
    static const String FW_VERSION;
    String getFWVersion() const override { return FW_VERSION; }
    static constexpr uint8_t GPIO_DEMAND = 3;
    uint8_t getGPIODemand() const override { return GPIO_DEMAND; }
    static constexpr uint8_t GPIO_SUPPLY = 0;
    uint8_t getGPIOSupply() const override { return GPIO_SUPPLY; }
 
    bool attachModule(uint8_t slot,uint8_t gpioStart) override;
    bool provideGPIOs() override;
    bool requestGPIOs() override;
    bool provideHardwareResources() override;
    bool requestHardwareResources() override;
    bool addInterfaces() override;
    bool addSPIDevice(SPIChip* spiDevice, bool allowOverride = false);

    bool setLoopFunction(std::function<void(uint8_t* response)> loopFunction){_loopFunction = loopFunction; return true; };

    bool initialize() override;
    bool preInitialize() override;

    void cycle(uint8_t* response) override;

    bool reset() override;

    Interface** getInterfaces() override { return _interfaces; }
    size_t getInterfaceCount() override { return INTERFACE_COUNT; }
    
    bool addUARTInterface();
    bool addI2CInterface(); //Implement later
    
    private:
        OutputDefinition* UART_RX_PIN;
        OutputDefinition* UART_TX_PIN;
        OutputDefinition* SPI_CS_PIN;
        SPIChip* _SPIBus = nullptr;
        static constexpr size_t INTERFACE_COUNT = 3; // Number of interfaces in the array
        Interface* _interfaces[INTERFACE_COUNT]; // representation of the physical interfaces clamps
        UARTInterface _uartInterface;
        I2CInterface _i2cInterface; 
        std::function<void(uint8_t* response)> _loopFunction = nullptr;
        //I2nterface* _i2cUnterface = nullptr;  //Implement later
        //SPIInterface* _spi = nullptr;
};
#endif