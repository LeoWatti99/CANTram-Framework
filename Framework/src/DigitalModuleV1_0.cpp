#include "DigitalModuleV1_0.h"
#include "CANTramCore.h"
#include "OutputDefinition.h"
#include "Debug.h"

const String DigitalModuleV1_0::HW_TYPE = "DigitalModuleV1.0";
const String DigitalModuleV1_0::HW_VERSION = "1.0";
const String DigitalModuleV1_0::FW_VERSION = "1.0";

/**
 * @brief Construct a new DigitalModuleV1_0 object.
 * @details Initializes internal state, output mask and constructs input/output IC driver and interface objects.
 * @param outputMask Bitmask selecting which channels are configured as outputs.
 * @return void
 */
DigitalModuleV1_0::DigitalModuleV1_0(uint8_t outputMask) :

                                                           // Prepare module configuration
                                                           OUTPUT_MASK(outputMask),

                                                           // Prepare module hardware
                                                           _Outputs(ISO1H816G()),
                                                           _Inputs(ISO1I813T()),
                                                           _digitalInputInterfaces({DigitalInput(), DigitalInput(), DigitalInput(), DigitalInput(), DigitalInput(), DigitalInput(), DigitalInput(), DigitalInput()}),
                                                           _digitalOutputInterfaces({DigitalOutput(), DigitalOutput(), DigitalOutput(), DigitalOutput(), DigitalOutput(), DigitalOutput(), DigitalOutput(), DigitalOutput()})

// Map Interfaces to hardware
// Input only channels
{
    // Input only
}

/**
 * @brief Attach module to a slot and assign GPIO start index.
 * @details Calls the base CANTramModule attach implementation and performs any DigitalModule-specific attach logic.
 * @param slot Slot index to attach the module to.
 * @param gpioStart Starting GPIO index assigned to this module.
 * @return true if attach succeeded, false otherwise.
 */
bool DigitalModuleV1_0::attachModule(uint8_t slot, uint8_t gpioStart)
{
    bool result = true;
    result &= CANTramModule::attachModule(slot, gpioStart);

    // Output setup

    
    return result;
}

/**
 * @brief Provide GPIO definitions to the core.
 * @details Placeholder: the digital module does not add additional GPIO definitions beyond core expectations.
 * @return true on success.
 */
bool DigitalModuleV1_0::provideGPIOs()
{
    return true;
}

/**
 * @brief Request and configure GPIOs used by the module.
 * @details Obtains CS output definitions from CANTramCore for outputs and inputs, configures them as outputs,
 *          and sets their initial state to HIGH (deselected). Logs errors when definitions are missing.
 * @return true if required GPIO definitions were obtained and configured, false otherwise.
 */
bool DigitalModuleV1_0::requestGPIOs()
{
    bool result = true;
    CS_OUTPUTS = CANTramCore::getOutputDefinition(GPIO_START);
    if (CS_OUTPUTS == nullptr)
    {
        ERROR_PRINTLN("[DigitalModuleV1_0] ERROR: Failed to get output definition at index " + String(GPIO_START) + " for output CS pin.");
        result = false;
    } else {
        CS_OUTPUTS->initialValue = HIGH; // Ensure CS pin is high on initialization
        if (CS_OUTPUTS->provider) CS_OUTPUTS->provider->configureOutput(CS_OUTPUTS, OUTPUT); // Configure as output
    }

    CS_INPUTS = CANTramCore::getOutputDefinition(GPIO_START + 1);
    if (CS_INPUTS == nullptr)
    {
        ERROR_PRINTLN("[DigitalModuleV1_0] ERROR: Failed to get output definition at index " + String(GPIO_START + 1) + " for input CS pin.");
        result = false;
    } else {
        CS_INPUTS->initialValue = HIGH; // Ensure CS pin is high on initialization
        if (CS_INPUTS->provider) CS_INPUTS->provider->configureOutput(CS_INPUTS, OUTPUT); // Configure as output
    }
    return result;
}

/**
 * @brief Provide hardware resources to CANTramCore.
 * @details Currently no additional hardware resources are attached by this module.
 * @return true on success.
 */
bool DigitalModuleV1_0::provideHardwareResources()
{
    return true;
}

/**
 * @brief Request hardware resources required by the module.
 * @details Placeholder for requesting shared hardware resources. Returns true by default.
 * @return true if requested resources were acquired, false otherwise.
 */
bool DigitalModuleV1_0::requestHardwareResources()
{
    return true;
}

/**
 * @brief Add and configure interfaces exposed by the module.
 * @details Populates the module's internal interface array with DigitalInput and DigitalOutput instances,
 *          sets interface names and validates them based on availability of CS pins and output mask.
 * @return true if interfaces were added and validated successfully, false otherwise.
 */
bool DigitalModuleV1_0::addInterfaces()
{
    bool validateInputs = CS_INPUTS != nullptr ? true : false;
    bool validateOutputs = CS_OUTPUTS != nullptr ? true : false;
    // Input only channels (I1-I4)
    for (int i = 0; i < 4; ++i)
    {
        _interfaces[i] = &_digitalInputInterfaces[7 - i]; // I8-I5
        _interfaces[i]->rename("I" + String(8-i));
        _interfaces[i]->validate(validateInputs);
    }
    // Input/Output channels (I4/Q1 - I1/Q4)
    uint8_t outputCount = 0; // Counter for output interfaces
    for (int i = 0; i < 4; i++)
    {
        if (OUTPUT_MASK & (1 << i))
        {
            // Configured as output
            _interfaces[4 + i] = &_digitalOutputInterfaces[i];
            _interfaces[4 + i]->rename("Q" + String(i + 1));
            _interfaces[4 + i]->validate(validateOutputs);
            outputCount++;
        }
        else
        {
            // Configured as input
            _interfaces[4 + i] = &_digitalInputInterfaces[3 - i];
            _interfaces[4 + i]->rename("I" + String(4 - i));
            _interfaces[4 + i]->validate(validateInputs);
        }
    }
    // Output only channels
    for (int i = 0; i < 4; ++i)
    {
        _interfaces[8 + i ] = &_digitalOutputInterfaces[i+4];
        _interfaces[8 + i]->rename("Q" + String(i + 5));
        _interfaces[8 + i]->validate(validateOutputs);
        outputCount++;
    }

    return validateInputs && validateOutputs;
}

/**
 * @brief Configure wire-break mask for input detection.
 * @details Iterates through channels and sets the wire-break detection mask on the input IC.
 *          Logs errors for any failure to apply the mask on individual channels.
 * @param mask Bitmask specifying which channels should have wire-break detection enabled.
 * @return void
 */
void DigitalModuleV1_0::setWireBreakMask(uint8_t mask)
{
    for (int i = 0; i < 8; i++)
    {
        bool activateMask = ((mask && OUTPUT_MASK) >> i) & 0x01;
        uint8_t success = _Inputs.setWireBreakMask(i, activateMask);
        if (success != ISO1I813T::STATUS_OK)
        {
            ERROR_PRINTLN("[DigitalModuleV1_0] ERROR: Failed to set wire break mask for channel " + String(i) + ". Error code: " + String(success));
        }
    }
}

/**
 * @brief Initialize the digital module hardware and ICs.
 * @details Initializes output and input ICs, configures external chip-select handling using the configured
 *          CS output providers, and starts IC drivers. Logs configuration state for debugging.
 * @return true if initialization succeeded, false otherwise.
 */
bool DigitalModuleV1_0::initialize()
{
    INFO_PRINTLN("[DigitalModuleV1_0] Initializing output IC...");
    _Outputs.activateExternalChipSelect();
    _Outputs.setCSControlCallback([this](bool state)
                                  { if(CS_OUTPUTS && CS_OUTPUTS->provider) CS_OUTPUTS->provider->setOutput(CS_OUTPUTS, state); });
    INFO_PRINTLN("[DigitalModuleV1_0] INFO: Output CS pin: " + (CS_OUTPUTS ? CS_OUTPUTS->toString() : String("undefined")));
    INFO_PRINTLN("[DigitalModuleV1_0] INFO: Output mask: 0b" + String(OUTPUT_MASK, BIN));
    _Outputs.begin();

    // Input setup

    INFO_PRINTLN("[DigitalModuleV1_0] Initializing input IC...");
    _Inputs.activateExternalChipSelect();
    _Inputs.setCSControlCallback([this](bool state)
                                 { if(CS_INPUTS && CS_INPUTS->provider) CS_INPUTS->provider->setOutput(CS_INPUTS, state); });
    INFO_PRINTLN("[DigitalModuleV1_0] INFO: Input CS pin: " + (CS_INPUTS ? CS_INPUTS->toString() : String("undefined")));
    _Inputs.begin();
    return true;
}

/**
 * @brief Periodic cycle handler called by the core.
 * @details Updates outputs based on interface values, writes outputs to the output IC, reads inputs
 *          from the input IC (with repeated fetches for stability), and updates input interface values.
 * @param response Optional pointer to a response buffer; may be nullptr if not used.
 * @return void
 */
void DigitalModuleV1_0::cycle(uint8_t *response)
{
    DEBUG_PRINTLN("[DigitalModuleV1_0] Cycling module...");

    // read output interfaces
    _outputStates = 0;
    for (int i = 0; i < 8; i++)
    {
        _outputStates |= (_digitalOutputInterfaces[i].getQ() << i);
    }

    // applay output mask
    _outputStates &= OUTPUT_MASK;

    // apply outputs
    _Outputs.setOutputs(_outputStates);

    // read inputs
    ISO1I813T::Data data = _Inputs.fetchData();
    _inputStates = data.inputs;
    _wireBreaks = data.wireBreaks;


    // apply values to input interfaces
    for (int i = 0; i < 8; i++)
    {
        _digitalInputInterfaces[i].setQ((_inputStates >> i) & 0x01);
        DEBUG_PRINTLN("[DigitalModuleV1_0] Input " + _digitalInputInterfaces[i].getName() + " state: " + String((_inputStates >> i) & 0x01));
    }
}

/**
 * @brief Set all digital outputs at once.
 * @details Updates the module's cached output state respecting the output mask and writes the values into the output interfaces.
 * @param outputs Byte containing 8 output bits (bit0..bit7).
 * @return uint8_t STATUS_OK on success.
 */
uint8_t DigitalModuleV1_0::setOutputs(uint8_t outputs)
{
    _outputStates = outputs & OUTPUT_MASK;
    for (int i = 0; i < 8; i++)
    {
        _digitalOutputInterfaces[i].setQ((_outputStates >> i) & 0x01);
    }
    return STATUS_OK;
}

/**
 * @brief Set a single digital output.
 * @details Sets or clears the specified output bit in the module's cached output state if that channel is configured as an output.
 * @param output Index of the output channel (0..7).
 * @param state Desired logical state for the channel (true = HIGH, false = LOW).
 * @return uint8_t STATUS_OK if operation succeeded, DEACTIVATED_CHANNEL if the channel is not configured as an output.
 */
uint8_t DigitalModuleV1_0::setOutput(uint8_t output, bool state)
{
    if (OUTPUT_MASK & (1 << output))
    {
        if (state)
        {
            _outputStates |= (1 << output);
        }
        else
        {
            _outputStates &= ~(1 << output);
        }
        return STATUS_OK;
    }
    return DEACTIVATED_CHANNEL;
}