#include "AnalogModuleV1_0.h"
#include "CANTramCore.h"
#include "PWMCore.h"
#include "Debug.h"

const String AnalogModuleV1_0::HW_TYPE = "AnalogModuleV1.0";
const String AnalogModuleV1_0::HW_VERSION = "1.0";
const String AnalogModuleV1_0::FW_VERSION = "1.0";

/**
 * @brief Attach module to a slot and set GPIO start.
 * @details Calls base class attach to register slot and gpio start. Module-specific initialization is minimal here.
 * @param slot Module slot index.
 * @param gpioStart Starting GPIO index assigned to this module.
 * @return true if base attach succeeded, false otherwise.
 */
bool AnalogModuleV1_0::attachModule(uint8_t slot, uint8_t gpioStart) {
    bool result = CANTramModule::attachModule(slot, gpioStart);
    return result;
}

/**
 * @brief Provide GPIO definitions to the core.
 * @details This module does not add additional GPIO definitions beyond those provided elsewhere,
 *          so this function currently returns true and acts as a placeholder for future additions.
 * @return true on success.
 */
bool AnalogModuleV1_0::provideGPIOs(){
    return true;
}

/**
 * @brief Request and configure GPIOs used by the module.
 * @details Retrieves the chip-select output definition and PWM output definitions from CANTramCore.
 *          Configures the CS pin as an output and sets its initial state high to deselect the external ADC chip.
 * @return true if all required GPIOs were obtained/configured, false if any were missing or misconfigured.
 */
bool AnalogModuleV1_0::requestGPIOs(){
    bool result = true;

    //Get the output definition for Inputs
    CS_INPUTS = CANTramCore::getOutputDefinition(GPIO_START+4); //Input CS pin is fifth pin on board
    if(CS_INPUTS == nullptr){
        WARNING_PRINTLN("[AnalogModuleV1_0] Failed to get output definition at index " + String(GPIO_START) + " for input CS pin.");
        result = false;
    }
    if(CS_INPUTS -> provider) {
        CS_INPUTS->initialValue = HIGH; // Ensure CS pin is high on initialization
        CS_INPUTS->provider->configureOutput(CS_INPUTS, OUTPUT); // Configure as output
    }

    //Get the output definitions for PWM outputs
    for(int i=0;i<4;i++){
        PWM_OUTPUTS[i] = CANTramCore::getOutputDefinition(GPIO_START + i);
        if(PWM_OUTPUTS[i] == nullptr){
            WARNING_PRINTLN("[AnalogModuleV1_0] Failed to get output definition at index " + String(GPIO_START + 1 + i) + " for PWM output "+ String(i+1));
            result = false;
            continue;
        }
        INFO_PRINTLN("[AnalogModuleV1_0] PWM output "+ String(i+1) + " assigned to " + PWM_OUTPUTS[i]->toString());
    }
    return result;
}

/**
 * @brief Provide hardware resources to the core.
 * @details Currently this module does not attach additional hardware resources (e.g., cores) to CANTramCore.
 * @return true on success.
 */
bool AnalogModuleV1_0::provideHardwareResources(){
    return true;
}

/**
 * @brief Request hardware resources required by the module.
 * @details Registers analog output interfaces and requests PWM channels from the PWM provider for each output.
 *          Validates interfaces when registration succeeds.
 * @return true if all requested hardware resources were successfully acquired and interfaces validated, false otherwise.
 */
bool AnalogModuleV1_0::requestHardwareResources(){
    INFO_PRINTLN("[AnalogModuleV1_0] Requesting hardware resources...");
    bool result = true;
    for(int i=0;i<4;i++){
        _analogOutputInterfaces[i] = AnalogOutput(_resolution);
        if(PWM_OUTPUTS[i] == nullptr) {
            WARNING_PRINTLN("[AnalogModuleV1_0] WARNING: PWM output "+ String(i+1) + " not defined, skipping PWM channel registration.");
            _analogOutputInterfaces[i].invalidate();
            result = false;
            continue;
        }
        PWMOutputProvider* provider = static_cast<PWMOutputProvider*>(PWM_OUTPUTS[i]->provider);
        if(provider== nullptr){
            ERROR_PRINTLN("[AnalogModuleV1_0] ERROR: No PWM provider for output "+ String(PWM_OUTPUTS[i]->toString()));
            _analogOutputInterfaces[i].invalidate();
            result = false;
            continue;
        } 

        //Register the PWM channel
        bool valid = provider->registerPWMChannel(PWM_OUTPUTS[i]);

        //Validate the interface if registration was successful
        if(valid) {
            _analogOutputInterfaces[i].validate();
            INFO_PRINTLN("[AnalogModuleV1_0] Analog output "+ String(i+1) + " validated.");
        }
        result &= valid;
    }
    return result;
}

/**
 * @brief Add interface objects to the module's interface list.
 * @details Constructs and names analog input and output interface objects and places pointers into the module interface array.
 *          Input interfaces are validated only if a valid CS output definition exists.
 * @return true if interfaces were added and validated where possible, false if any required interface validation failed.
 */
bool AnalogModuleV1_0::addInterfaces() {
    bool result=true;
    //
    for (int i = 0; i < 4; ++i) {
        //already constructed in requestHardwareResources
        _analogOutputInterfaces[i].rename("AQ" + String(i + 1));
        _interfaces[i] = &_analogOutputInterfaces[i];
    }

    for (int i = 0; i < 4; ++i) {
        _analogInputInterfaces[i] = AnalogInput(_resolution);
        _analogInputInterfaces[i].rename("AI" + String(i + 1));
        _interfaces[4 + i] = &_analogInputInterfaces[i];

        if(CS_INPUTS == nullptr) {
            WARNING_PRINTLN("[AnalogModuleV1_0] WARNING: Input CS pin not defined, input interfaces will not be validated.");
            result=false;
            continue;
        }
        INFO_PRINTLN("[AnalogModuleV1_0] Analog input "+ String(i+1) + " validated.");
        _analogInputInterfaces[i].validate(); //Validate if CS pin is valid
        
    }

    return result;
}

/**
 * @brief Initialize the analog module and external ADC chip interface.
 * @details Activates external chip-select management on the input driver, installs a CS control callback that uses the module's CS output provider,
 *          and starts the input driver. Logs an error if initialization fails.
 * @return true if initialization succeeded, false otherwise.
 */
bool AnalogModuleV1_0::initialize() {
    INFO_PRINTLN("[AnalogModuleV1_0] Initializing module...");

    //Initialize the inputs
    bool result = true;
    _Inputs.activateExternalChipSelect();
    _Inputs.setCSControlCallback([this](bool state) {
        CS_INPUTS->provider->setOutput(CS_INPUTS, state);
    });
    if(!(result = _Inputs.begin())){
        ERROR_PRINTLN("[AnalogModuleV1_0] ERROR: Failed to initialize input chip.");
    }

    
    


    return result;
}

/**
 * @brief Periodic cycle handler called by the core.
 * @details Performs a burst read from the external ADC chip, logs and validates the status, updates analog input interface values,
 *          and propagates PWM output values to the PWM providers for validated outputs.
 * @param response Optional pointer to a response buffer where the module can write response bytes; may be nullptr if unused.
 */
void AnalogModuleV1_0::cycle(uint8_t* response) {
    //Read all inputs
    MAX22531::BurstResponse burst = _Inputs.burstRead(true);
    if(burst.status != MAX22531::STATUS_OK){
        ERROR_PRINTLN("[AnalogModuleV1_0] ERROR: ERROR reading inputs, status code: " + String(burst.status));
    }
    
    //Debug input values
    DEBUG_PRINTLN("[AnalogModuleV1_0] ADC1: " + String(burst.adc1));
    DEBUG_PRINTLN("[AnalogModuleV1_0] ADC2: " + String(burst.adc2));
    DEBUG_PRINTLN("[AnalogModuleV1_0] ADC3: " + String(burst.adc3));
    DEBUG_PRINTLN("[AnalogModuleV1_0] ADC4: " + String(burst.adc4));

    //Update input interfaces
    _analogInputInterfaces[0].setQ(burst.adc1);
    _analogInputInterfaces[1].setQ(burst.adc2);
    _analogInputInterfaces[2].setQ(burst.adc3);
    _analogInputInterfaces[3].setQ(burst.adc4);
    
   
    //Update output interfaces
    for(int i=0;i<4;i++){
        if(_analogOutputInterfaces[i].isValid()){

            //Get the output provider
            PWMOutputProvider* provider = static_cast<PWMOutputProvider*>(PWM_OUTPUTS[i]->provider);

            //Set the PWM value, if provider is valid
            if(provider) provider->setPWM(PWM_OUTPUTS[i], _analogOutputInterfaces[i].getQ());

        }
    }

}

/**
 * @brief Reset the module and its interfaces.
 * @details Calls base reset, attempts to reset the external input chip, and clears interface values to zero.
 * @return true if reset operations succeeded, false if any reset failed.
 */
bool AnalogModuleV1_0::reset() {
    CANTramModule::reset();
    INFO_PRINTLN("[AnalogModuleV1_0] Resetting module...");
    //Reset input chip
    bool result = true;
    if(_Inputs.HWreset()!=MAX22531::STATUS_OK) result &= false;
    
    //Reset all interfaces
    for(int i=0;i<INTERFACE_COUNT;i++){
        if(_interfaces[i]) _interfaces[i]->setQ(0);
    }
    return result;
}