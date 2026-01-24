#include "BusModule.h"
#include "OutputDefinition.h"
#include "Debug.h"

const String BusModuleV1_0::HW_TYPE = "BusModuleV1.0";
const String BusModuleV1_0::HW_VERSION = "1.0";
const String BusModuleV1_0::FW_VERSION = "1.0";

/**
 * @brief Attach this module to a slot and GPIO start index.
 * @details Calls the base CANTramModule attach implementation and performs any module-specific attach logic.
 * @param slot Slot index to attach the module to.
 * @param gpioStart Starting GPIO index assigned to this module.
 * @return true if the attach operation was successful, false otherwise.
 */
bool BusModuleV1_0::attachModule(uint8_t slot, uint8_t gpioStart)
{
    bool result = true;
    result &= CANTramModule::attachModule(slot, gpioStart);

    return result;

}

/**
 * @brief Provide GPIO definitions to the core.
 * @details Placeholder for BusModule-specific GPIO provisioning. Currently does not register additional GPIOs.
 * @return true on success.
 */
bool BusModuleV1_0::provideGPIOs()
{
    return true;
}

/**
 * @brief Request GPIO definitions from CANTramCore and validate pins for module interfaces.
 * @details Retrieves UART RX/TX and SPI CS output definitions from the core, validates they are not on shift registers
 *          and logs the assigned pins. Invalid pins will cause related interfaces to be invalidated.
 * @return true if all required GPIO definitions were found and valid, false otherwise.
 */
bool BusModuleV1_0::requestGPIOs()
{

    // Lambda to check if pin is suitable for RX
    auto checkRXPin = [&](uint8_t index) -> bool
    {
        
        UART_RX_PIN = CANTramCore::getOutputDefinition(index);
        if (UART_RX_PIN == nullptr)
        {
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: Failed to get output definition at index " + String(index) + " for UART RX pin.");
            return false;
        }
        if(UART_RX_PIN->isShift){
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: UART RX pin cannot be on a shift register (index " + String(index) + ").");
            return false;
        }
        INFO_PRINTLN("[BusModuleV1_0] INFO: UART RX pin " + UART_RX_PIN->toString() +" is suitable.");
        return true;
    };
    bool resultRX = checkRXPin(GPIO_START);

    // Lambda to check if pin is suitable for TX
    auto checkTXPin = [&](uint8_t index) -> bool
    {
        UART_TX_PIN = CANTramCore::getOutputDefinition(index);
        if (UART_TX_PIN == nullptr)
        {
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: Failed to get output definition at index " + String(index) + " for UART TX pin.");
            return false;
        }
        if(UART_TX_PIN->isShift){
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: UART TX pin cannot be on a shift register (index " + String(index) + ").");
            return false;
        }
        if(UART_TX_PIN->inputOnly){
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: UART TX pin cannot be input-only (index " + String(index) + ").");
            return false;
        }
        INFO_PRINTLN("[BusModuleV1_0] INFO: UART TX pin " + UART_TX_PIN->toString() +" is suitable.");
        return true;
    };
    bool resultTX = checkTXPin(GPIO_START + 1);

    // Invalidate UART interface if any of the pins are invalid
    if (!resultRX || !resultTX)
    {
        ERROR_PRINTLN("[BusModuleV1_0] ERROR: UART pins invalid. UART interface cannot be used.");
        _uartInterface.invalidate();
    }

    // Setup SPI CS pin
    bool resultSPI = true;
    SPI_CS_PIN = CANTramCore::getOutputDefinition(GPIO_START + 2);
    if (SPI_CS_PIN == nullptr)
    {
        ERROR_PRINTLN("[BusModuleV1_0] ERROR: Failed to get output definition at index " + String(GPIO_START + 2) + " for SPI CS pin.");
        resultSPI = false;
    }
    if (resultSPI)
        INFO_PRINTLN("[BusModuleV1_0] INFO: SPI CS pin: " + SPI_CS_PIN->toString());

    // return false if any of the pins are invalid
    return (resultRX && resultTX && resultSPI);
}

/**
 * @brief Provide hardware resources to CANTramCore.
 * @details Placeholder for attaching hardware resources (UART, I2C, SPI, etc.) that BusModule may provide.
 * @return true on success.
 */
bool BusModuleV1_0::provideHardwareResources()
{
    return true;
}

/**
 * @brief Request hardware resources required by the BusModule.
 * @details Attempts to acquire a UART hardware resource from CANTramCore and assigns it to the module's UART interface.
 *          If no UART resource is available the UART interface is invalidated.
 * @return true if required resources were successfully acquired, false otherwise.
 */
bool BusModuleV1_0::requestHardwareResources()
{
    
    // Request UART hardware resource
    bool uartResult = false;
    if(_uartInterface.isInvalid()) WARNING_PRINTLN("[BusModuleV1_0] UART interface is invalid, skipping UART hardware resource request.");   
    if (!_uartInterface.isInvalid())
    {
        INFO_PRINTLN("[BusModuleV1_0] Requesting UART hardware resource...");

        auto assignUartResource = [&]() -> bool
        {
            HardwareResource *uartResource = CANTramCore::useHardwareResource(HardwareResource::Type::UART);
            if (uartResource == nullptr)
            {
                ERROR_PRINTLN("[BusModuleV1_0] ERROR: No UART hardware resource available.");
                _uartInterface.invalidate();
                return false;
            }
            UARTCore* uartCore = static_cast<UARTCore*>(uartResource);
            if (uartCore == nullptr)
            {
                ERROR_PRINTLN("[BusModuleV1_0] ERROR: UART hardware resource is not a UARTCore.");
                _uartInterface.invalidate();
                return false;
            }
            _uartInterface.setUartCore(uartCore);
            return true;
        };
        uartResult = assignUartResource();
        INFO_PRINTLN("[BusModuleV1_0] INFO: UART hardware resource assigned.");
    }

    //I2C
    bool i2cResult = false;
    auto assignI2CResource = [&]() -> bool
    {
        HardwareResource *i2cResource = CANTramCore::useHardwareResource(HardwareResource::Type::I2C);
        if (i2cResource == nullptr)
        {
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: No I2C hardware resource available.");
            _i2cInterface.invalidate(); // Invalidate I2C interface
            return false;
        }
        I2CCore* i2cCore = static_cast<I2CCore*>(i2cResource);
        if (i2cCore == nullptr)
        {
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: I2C hardware resource is not an I2CCore.");
            _i2cInterface.invalidate(); // Invalidate I2C interface
            return false;
        }
        _i2cInterface.setI2CCore(i2cCore);
        return true;
    };

    i2cResult= assignI2CResource();
    return true;
}

/**
 * @brief Periodic cycle callback for the BusModule.
 * @details Called by the core to perform periodic tasks. Implement module-specific cyclic behavior here.
 * @param response Optional pointer to a response buffer where the module may write response bytes. May be nullptr if unused.
 */
void BusModuleV1_0::cycle(uint8_t *response)
{
    if(_loopFunction){
        _loopFunction(response);
        DEBUG_PRINTLN("[BusModuleV1_0] Executed callback loop function of BusModuleV1_0.");
    }
}

/**
 * @brief Add interfaces provided by the BusModule to the module interface list.
 * @details Adds UART and I2C interfaces (I2C stubbed) and logs the operation.
 * @return true if interfaces were added successfully, false otherwise.
 */
bool BusModuleV1_0::addInterfaces()
{
    INFO_PRINTLN("[BusModuleV1_0] Adding interfaces...");

    bool result = true;
    // Setup UART interface
    result &= addUARTInterface();
    result &= addI2CInterface(); // Implement later

    return result;
}

/**
 * @brief Add and configure the UART interface for this module.
 * @details Inserts the UART interface into the module interface array, validates associated pins and configures UART core pins.
 * @return true if UART interface setup succeeded, false otherwise.
 */
bool BusModuleV1_0::addUARTInterface()
{
    _interfaces[0] = &_uartInterface;
    _uartInterface.rename("UART"+String(SLOT));
    _uartInterface.setBaudrate(9600); // Default baudrate
    _uartInterface.setDataBits(UARTCore::DataBits::UART_DATA_8_BITS);
    _uartInterface.setParity(UARTCore::Parity::UART_PARITY_NONE);
    _uartInterface.setStopBits(UARTCore::StopBits::UART_STOP_BITS_1);

    INFO_PRINTLN("[BusModuleV1_0] INFO: Added UART interface.");
    return true;
}

/**
 * @brief Add and configure the I2C interface for this module.
 * @details Inserts the I2C interface into the module interface array.
 * @return true if I2C interface setup succeeded, false otherwise.
 */
bool BusModuleV1_0::addI2CInterface()
{
    _interfaces[1] = &_i2cInterface;
    _i2cInterface.rename("I2C"+String(SLOT));
    INFO_PRINTLN("[BusModuleV1_0] INFO: Added I2C interface.");
    return true;
}

bool BusModuleV1_0::addSPIDevice(SPIChip* spiDevice, bool allowOverride)
{
    if(spiDevice == nullptr)
    {
        ERROR_PRINTLN("[BusModuleV1_0] ERROR: SPI device pointer is null.");
        return false;
    }
    if(_SPIBus != nullptr)
    {
        if(!allowOverride)
        {
            ERROR_PRINTLN("[BusModuleV1_0] ERROR: SPI bus already has a device assigned and overriding is not allowed.");
            return false;
        }
        WARNING_PRINTLN("[BusModuleV1_0] Overriding existing SPI device!");
    }
    _SPIBus = spiDevice;
    INFO_PRINTLN("[BusModuleV1_0] External SPI device added.");
    return true;
}
bool BusModuleV1_0::initialize()
{
    //Busmodule is pre-initialized since it requires only external resources 
    return true;
}

bool BusModuleV1_0::preInitialize()
{
    DEBUG_PRINTLN("[BusModuleV1_0] Pre-Initializing BusModuleV1_0...");
    bool result = true;

    // Initialize UART interface
    if(!_uartInterface.isInvalid())
    {
        DEBUG_PRINTLN("[BusModuleV1_0] Pre-Initializing UART interface...");
        result &= _uartInterface.getUartCore()->setPins(UART_RX_PIN->pinOrBit, UART_TX_PIN->pinOrBit);
        if(result)
            _uartInterface.validate();
        else
            _uartInterface.invalidate();
    }
    else
    {
        WARNING_PRINTLN("[BusModuleV1_0] UART interface is invalid, skipping initialization.");
    }

    // Initialize I2C interface
    if(!_i2cInterface.isInvalid())
    {
        DEBUG_PRINTLN("[BusModuleV1_0] Initializing I2C interface...");
        result &= _i2cInterface.begin();
        if(result)
            _i2cInterface.validate();
        else
            _i2cInterface.invalidate();
    }
    else
    {
        WARNING_PRINTLN("[BusModuleV1_0] I2C interface is invalid, skipping initialization.");
    }

    return result;
}

bool BusModuleV1_0::reset()
{
    
    DEBUG_PRINTLN("[BusModuleV1_0] Resetting BusModuleV1_0...");
    CANTramModule::reset();
    bool result = true;

    _loopFunction = nullptr;
    UART_RX_PIN = nullptr;
    UART_TX_PIN = nullptr;
    SPI_CS_PIN = nullptr;
    _SPIBus = nullptr;

    return result;
}