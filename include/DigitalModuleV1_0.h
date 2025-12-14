#ifndef DIGITALMODULEV1_0_H
#define DIGITALMODULEV1_0_H

#include <Arduino.h>
#include "CANTramModule.h"
#include "ISO1H816G.h"
#include "ISO1I813T.h"
#include "Interface.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "OutputDefinition.h"
#include "Debug.h"




/* Steckerbelegung des Digitalmoduls V1.0
J3
PIN   | Signal       | Funktion
---------------------------------------------------------
1     | I8           | Eingang
2     | I7           | Eingang
3     | I6           | Eingang
4     | I5           | Eingang

J5
PIN   | Signal       | Funktion
---------------------------------------------------------
5     | I4 / Q1      | Eingang ODER Ausgang
6     | I3 / Q2      | Eingang ODER Ausgang
7     | I2 / Q3      | Eingang ODER Ausgang
8     | I1 / Q4      | Eingang ODER Ausgang

J6
PIN   | Signal       | Funktion
---------------------------------------------------------
9     | Q5           | Ausgang
10    | Q6           | Ausgang
11    | Q7           | Ausgang
12    | Q8           | Ausgang


*/

class DigitalModuleV1_0 : public CANTramModule {
    public:

    //Module information
    static const String HW_TYPE;
    String getHWType() const override { return HW_TYPE; }
    static const String HW_VERSION;
    String getHWVersion() const override { return HW_VERSION; }
    static const String FW_VERSION;
    String getFWVersion() const override { return FW_VERSION; }
    static constexpr uint8_t GPIO_DEMAND = 2;
    uint8_t getGPIODemand() const override { return GPIO_DEMAND; }
    static constexpr uint8_t GPIO_SUPPLY = 0;
    uint8_t getGPIOSupply() const override { return GPIO_SUPPLY; }

    /**
     * @brief Construct a DigitalModuleV1_0 instance.
     * @details Create the digital module and configure which channels act as outputs using the outputMask.
     * @param outputMask Bitmask where a set bit configures the corresponding channel as an output.
     * @return void
     */
    DigitalModuleV1_0(uint8_t outputMask = 0xF0);

    /**
     * @brief Attach the module to a slot and assign GPIO range.
     * @details Performs CANTramModule base attach logic and any module specific attach-time configuration.
     * @param slot Slot index to attach the module to.
     * @param gpioStart Starting GPIO index assigned to this module.
     * @return bool true if attach and provisioning succeeded, false otherwise.
     */
    bool attachModule(uint8_t slot, uint8_t gpioStart) override;

    /**
     * @brief Provide GPIO definitions to the core.
     * @details Declare any GPIOs this module provides to CANTramCore. Called during attachment.
     * @return bool true if GPIO definitions were provided successfully, false otherwise.
     */
    bool provideGPIOs() override;

    /**
     * @brief Request and configure GPIOs from the core.
     * @details Obtain OutputDefinition pointers for CS pins and configure their initialValue and mode.
     *          This method must keep the configuration and initialValue assignment for correct operation.
     * @return bool true if required GPIO definitions were obtained and configured, false otherwise.
     */
    bool requestGPIOs() override;

    /**
     * @brief Provide hardware resources to the core.
     * @details Attach any HardwareResource instances offered by this module so other modules can request them.
     * @return bool true if hardware resources were provided successfully, false otherwise.
     */
    bool provideHardwareResources() override;

    /**
     * @brief Request hardware resources required by this module.
     * @details Acquire shared hardware resources (if any) from CANTramCore for module operation.
     * @return bool true if required resources were granted, false otherwise.
     */
    bool requestHardwareResources() override;

    /**
     * @brief Add and validate interfaces provided by the module.
     * @details Populate the internal Interface* array with DigitalInput/DigitalOutput instances and validate them.
     * @return bool true if interfaces were added and validated successfully, false otherwise.
     */
    bool addInterfaces() override;

    /**
     * @brief Initialize the module and its IC drivers.
     * @details Configure and start the input/output driver ICs, set up external CS handling and prepare for operation.
     * @return bool true if initialization completed successfully, false otherwise.
     */
    bool initialize() override;

    /**
     * @brief Periodic cycle callback.
     * @details Called by the core to perform cyclic tasks: update outputs, read inputs and update interface values.
     * @param response Optional pointer to a response buffer where response bytes may be written (may be nullptr).
     * @return void
     */
    void cycle(uint8_t* response) override;

    /**
     * @brief Configure wire-break detection mask.
     * @details Apply the provided mask to enable/disable wire-break detection per channel on the input IC.
     * @param mask Bitmask indicating channels for which wire-break detection should be active.
     * @return void
     */
    void setWireBreakMask(uint8_t mask=0);

    //Interfaces
    /**
     * @brief Get pointer to the module's interface array.
     * @details Returns a pointer to the internal array of Interface pointers for enumeration by the core.
     * @return Interface** Pointer to the first element of the interfaces array.
     */
    Interface** getInterfaces() override { return _interfaces; }

    /**
     * @brief Get number of interfaces provided by this module.
     * @details Returns the compile-time count of interface slots for this module.
     * @return size_t Number of interface pointers in the module.
     */
    size_t getInterfaceCount() override { return INTERFACE_COUNT; }

    /**
     * @brief Set all digital outputs at once.
     * @details Update the module's outputs cache and corresponding output interfaces, respecting the output mask.
     * @param outputs Byte containing 8 output bits (bit0..bit7).
     * @return uint8_t STATUS_OK on success.
     */
    uint8_t setOutputs(uint8_t outputs);

    /**
     * @brief Set a single digital output.
     * @details Set or clear a single output bit in the module's cached output state if the channel is configured as output.
     * @param output Index of the output channel (0..7).
     * @param state Desired logical state for the channel (true = HIGH, false = LOW).
     * @return uint8_t STATUS_OK if operation succeeded, DEACTIVATED_CHANNEL if channel is not configured as output.
     */
    uint8_t setOutput(uint8_t output, bool state);

    /**
     * @brief Get current input states.
     * @details Returns the cached input state byte where each bit represents an input channel state.
     * @return uint8_t Byte with input states.
     */
    uint8_t getInputs() const { return _inputStates; }

    /**
     * @brief Get current wire-break status.
     * @details Returns the cached wire-break status byte where each bit represents a wire-break flag for a channel.
     * @return uint8_t Byte with wire-break flags.
     */
    uint8_t getWireBreaks() const { return _wireBreaks; }

    //Error codes
    enum ERROR_CODES{
        STATUS_OK = 0,
        DEACTIVATED_CHANNEL = 11
    };
   
    private:
        static constexpr size_t INTERFACE_COUNT = 12; // Number of interfaces in the array
        ISO1I813T _Inputs;
        ISO1H816G _Outputs;
        OutputDefinition* CS_INPUTS; // Chip select pin for ISO1H816G
        OutputDefinition* CS_OUTPUTS; // Chip select pin for ISO1I813T
        const uint8_t OUTPUT_MASK; // Mask for valid outputs
        uint8_t _outputStates = 0; // Current states of outputs
        uint8_t _wireBreaks = 0;
        uint8_t _inputStates = 0; // Current states of inputs
        DigitalInput _digitalInputInterfaces[8]; // Array to hold 8 DigitalInput objects
        DigitalOutput _digitalOutputInterfaces[8]; // Array to hold 8 DigitalOutput objects
        Interface* _interfaces[INTERFACE_COUNT]; // representation of the physical interfaces clamps
        
};
#endif





