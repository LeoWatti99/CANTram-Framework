#include <CANTramCore.h>
#include <Arduino.h>
#include "Debug.h"

//Initialize static members
CANTramModule* CANTramCore::modules[CANTramCore::MAX_MODULES];
HardwareResource* CANTramCore::hardwareResources[CANTramCore::MAX_HARDWARE_RESOURCES] = {nullptr};
OutputDefinition* CANTramCore::outputDefinitionTable[CANTramCore::MAX_GPIO];
uint8_t CANTramCore::moduleCount = 0;
uint8_t CANTramCore::providedGPIOs = 0;
uint8_t CANTramCore::usedGPIOs = 0;
CANCore::Baudrate CANTramCore::DEFAULT_CAN_BAUDRATE = CANCore::BR_500K;

bool CANTramCore::outputDefinitionTableInitialized = false;
 
/**
 * @brief Call this function to attach a module to the core.
 * @details This function registers the module, assigns it a slot and GPIO start index, and registers its interfaces and hardware resources with the core. It also checks for maximum module limits and GPIO usage.
 * 
 * @param module Pointer to the module to be attached
 * @param index [Optional] Slot index for the module 
 * @param gpioStart [Optional] GPIO start index for the module 
 * @return true if the module was successfully attached, false otherwise
 */
bool CANTramCore::attachModule(CANTramModule *module, int8_t index,int8_t gpioStart) {
    INFO_PRINTLN("[CANTramCore] Attaching module " + module->getHWType()+ "...");

    //Check for unrecommended use of index parameter
    if(index != -1){
        WARNING_PRINTLN("[CANTramCore] The method attachModule(...) with index parameter is meant for testing only. Please use attachModule(...) without the index parameters.");
    }

    bool overriding = false;    //Flag to indicate if we are overwriting an existing module
    if(index != -1 && modules[index] != nullptr) {  //Check for existing module at specified index
        overriding = true;
        CRITICAL_ERROR_PRINTLN("[CANTramCore] Module slot " + String(index) + " already occupied by " + modules[index]->getHWType() +".");
        return false;
    }

    if(index == -1) {       //Default value, use next available slot
        index = moduleCount;
    }
    DEBUG_PRINTLN("[CANTramCore] Try attaching module at index " + String(index));
    
    if (index >= MAX_MODULES) {
        CRITICAL_ERROR_PRINTLN("[CANTramCore] Maximum module limit of " + String(MAX_MODULES) +" reached.");
        return false;
    }
    
    modules[index] = module;
    bool result = module->attachModule(index, gpioStart);
    
    moduleCount++; //Increase module count only after successful attachment
    if(!result) {
        WARNING_PRINTLN("[CANTramCore] Attaching incomplete: Not all features of " + String(module->getHWType()) + " in slot " + String(index) + " could be initialized. ");
        return false;
    }
    INFO_PRINTLN("[CANTramCore] INFO: Successfully attached "+ module->getHWType() +" at slot " + String(module->getSlot()) +", GPIO start: " + String(module->getGPIOStart()));
    return true;
}

/**
 * @brief Call this function to attach a module to next available slot of the core.
 * 
 * @param module Pointer to the module to be attached
 * @return true if the module was successfully attached, false otherwise
 */
bool CANTramCore::attachModule(CANTramModule *module) {
    return attachModule(module, -1, usedGPIOs);
}
/**
 * @brief Call this function to add multiple output definitions by using their instances to the core.
 * 
 * @param def Pointer to the array of output definitions
 * @param size Number of output definitions to add
 * @return CANTramCoreError Error code indicating success or failure
 */
CANTramCoreError CANTramCore::addOutputDefinitions(OutputDefinition def[], uint8_t size) {
    // Find the first empty slot in the output definition table
    DEBUG_PRINTLN("[CANTramCore] Adding " + String(size) + " output definitions starting at index " + String(providedGPIOs));
    Serial.println("[CANTramCore] Adding " + String(size) + " output definitions starting at index " + String(providedGPIOs));
    CANTramCoreError err=CANTramCoreError::CAN_TRAM_OK;
    for (uint8_t i = 0; i < size; i++) {
        err = addOutputDefinition(&def[i]);
    }
    return err;
}

/**
 * @brief Call this function to add multiple output definitions to the core.
 * 
 * @param def Pointer to the array of output definition pointers
 * @param size Number of output definitions to add
 * @return CANTramCoreError Error code indicating success or failure
 */
CANTramCoreError CANTramCore::addOutputDefinitionPointers(OutputDefinition* def[], uint8_t size) {
    // Find the first empty slot in the output definition table
    CANTramCoreError err=CANTramCoreError::CAN_TRAM_OK;
    for (uint8_t i = 0; i < size; i++) {
        err = addOutputDefinition(def[i]);
    }
    return err;
}

/**
 * @brief Call this function to add a single output definition to the core.
 * 
 * @param def Pointer to the output definition to add
 * @param index Index at which to add the output definition
 * @param overwrite Flag indicating whether to overwrite an existing definition
 * @return CANTramCoreError Error code indicating success or failure
 */
 
CANTramCoreError CANTramCore::addOutputDefinition(OutputDefinition* def,uint8_t index,bool overwrite) {
    // Find the first empty slot in the output definition table
    if(index >= MAX_GPIO) {
        ERROR_PRINTLN("[CANTramCore] ERROR: Output definition index " + String(index) + " exceeds maximum of " + String(MAX_GPIO) + ".");
        return CANTramCoreError::MAX_GPIOS_EXCEEDED;
    }
    if(def == nullptr) {
        CRITICAL_ERROR_PRINTLN("[CANTramCore] ERROR: Attempted to add null output definition at index " + String(index) + ".");   
    }
    if(outputDefinitionTable[index] != nullptr) {
        if(!overwrite){
            ERROR_PRINTLN("[CANTramCore] ERROR: Output definition index " + String(index) + " already occupied. Use overwrite=true to overwrite existing definition.");
            return CANTramCoreError::GPIO_OVERWRITE;
        }
        WARNING_PRINTLN("[CANTramCore] Overwriting existing output definition at index " + String(index) + ": " + outputDefinitionTable[index]->toString());
    }
    INFO_PRINTLN("[CANTramCore] Adding output definition at index " + String(index) + ": " + def->toString());
    outputDefinitionTable[index] = def;
    providedGPIOs++;
    return CANTramCoreError::CAN_TRAM_OK;
}

/**
 * @brief Call this function to add a single output definition to the core.
 * @details This function searches for the first available slot in the output definition table and adds the provided definition there. If no slots are available, it returns an error.
 * 
 * @param def Pointer to the output definition to add
 * @return CANTramCoreError Error code indicating success or failure
 */
CANTramCoreError CANTramCore::addOutputDefinition(OutputDefinition* def) {
    // Find the first empty slot in the output definition table
    for (uint8_t i = 0; i < MAX_GPIO; i++) {
        if (outputDefinitionTable[i] == nullptr) {
            return addOutputDefinition(def, i);
        }
    }
    ERROR_PRINTLN("[CANTramCore] ERROR: No empty slot available in output definition table.");
    return CANTramCoreError::MAX_GPIOS_EXCEEDED;
}

/**
 * @brief Call this function to retrieve a specific output definition without affecting the used count.
 * @details This function returns a pointer to the output definition at the specified index. If the index is out of bounds or if no definition exists at that index, it returns nullptr and logs an error. Use this command, if you just need information about this output definition but don't want to asign a function to it.
 * @param nr Index of the output definition to retrieve
 * @return Pointer to the requested output definition, or nullptr if not found
 */
OutputDefinition* CANTramCore::getOutputDefinition(uint8_t nr) {
    if(nr >= MAX_GPIO) {
        CRITICAL_ERROR_PRINTLN("[CANTramCore] ERROR: Output definition index " + String(nr) + " exceeds maximum of " + String(MAX_GPIO) + ".");
        return nullptr;
    }
    if(outputDefinitionTable[nr] == nullptr) {
        CRITICAL_ERROR_PRINTLN("[CANTramCore] ERROR: No output definition found at index " + String(nr) + ".");
        return nullptr;
    }
    return outputDefinitionTable[nr];
}

/**
 * @brief Call this function to retrieve a specific output definition and raise the count of used output definitions.
 * @details Use getOutputDefinition(...) instead, if you don't want to assign the output to any module.
 * @param nr Index of the output definition to retrieve
 * @return Pointer to the requested output definition, or nullptr if not found
 */
OutputDefinition* CANTramCore::useOutputDefinition(uint8_t nr) {
    usedGPIOs++;
    return getOutputDefinition(nr);
}


/**
 * @brief Call this function to attach a hardware resource to the core. E.g., I2C, SPI, UART, PWM
 * 
 * @param resource Pointer to the hardware resource to attach
 * @return true if the resource was attached successfully, false otherwise
 */
bool CANTramCore::attachHardwareResource(HardwareResource* resource) {
    for(int i=0;i<MAX_HARDWARE_RESOURCES;i++){
        if(hardwareResources[i] == nullptr){
            hardwareResources[i] = resource;
            DEBUG_PRINTLN("[CANTramCore] Attached hardware resource of type " + String((int)resource->getType()) + " at index " + String(i) + ".");
            return true;
        }
    }
    ERROR_PRINTLN("[CANTramCore] ERROR: Maximum hardware resource limit reached.");
    return false;
}


/**
 * @brief Call this function periodically to allow modules to perform cyclic tasks.
 * @details This function iterates through all attached modules and calls their cycle method. It is typically called in the main loop of the program.
 * 
 * @return uint8_t Status code (0 for success)
 */
uint8_t CANTramCore::loop() {
    DEBUG_PRINTLN("[CANTramCore] Running core loop...");
    for(uint8_t i=0;i<MAX_MODULES;i++) {
        if(modules[i] != nullptr) {
            modules[i]->cycle(nullptr);
        }
    }
    return 0;
}

/**
 * @brief Call this function to initialize the CANTram core and therefore the controller.
 * @details This function initializes all attached modules and sets initial GPIO values as defined in their output definitions. It should be called after all modules have been attached and configured.
 * 
 * @return true if initialization was successful, false otherwise
 */
bool CANTramCore::initialize() {
    INFO_PRINTLN("[CANTramCore] Initializing core...");

    bool result = true;
    INFO_PRINTLN("[CANTramCore] Pre-initializing modules...");
    for(int i=0;i<MAX_MODULES;i++) {
        if(modules[i] == nullptr) continue;
        INFO_PRINTLN("[CANTramCore] Pre-initializing module " + String(modules[i]->getHWType()) + " in slot " + String(modules[i]->getSlot()) + "...");
        result &= modules[i]->preInitialize();
    }
    INFO_PRINTLN("[CANTramCore] Pre-initialization complete.");
    
    //set gpio initial values
    INFO_PRINTLN("[CANTramCore] Setting initial GPIO values...");
    for(int i=0;i<MAX_GPIO;i++) {
        OutputDefinition* def = outputDefinitionTable[i];
        if(def == nullptr) continue;   //Skip if no output is registered
        if(def->isShift) continue; //Skip if output is on shift register

        //set native gpio initial values
        bool initialState = def->initialValue;
        result &= def->provider->setOutput(def, initialState);
        INFO_PRINTLN("[CANTramCore] Initialized pin GPIO"+String(def->pinOrBit)+"(INDEX: "+String(i)+") to "+String(initialState)); 
    }

    //initialize i2c shift registers
    INFO_PRINTLN("[CANTramCore] Setting initial shift register values...");
    for(int i=0;i<MAX_GPIO;i++) {
        OutputDefinition* def = outputDefinitionTable[i];
        if(def == nullptr) continue;   //Skip if no output is registered
        if(!def->isShift) continue; //Skip if output is not on shift register

        //set i2c shift register initial values
        bool initialState = def->initialValue;
        result &= def->provider->setOutput(def, initialState);
        INFO_PRINTLN("[CANTramCore] Initialized shift register pin "+String(def->pinOrBit)+"(INDEX: "+String(i)+") to "+String(initialState)); 
    }

    //initialize modules
    for(int i=0;i<MAX_MODULES;i++) {
        if(modules[i] == nullptr) continue;
        INFO_PRINTLN("[CANTramCore] Initializing module " + String(modules[i]->getHWType()) + " in slot " + String(modules[i]->getSlot()) + "...");
        bool initResult = modules[i]->initialize();
        if(!initResult) {
            WARNING_PRINTLN("[CANTramCore] WARNING: Module " + String(modules[i]->getHWType()) + " in slot " + String(modules[i]->getSlot()) + " failed to initialize.");
        } else {
            INFO_PRINTLN("[CANTramCore] INFO: Module " + String(modules[i]->getHWType()) + " in slot " + String(modules[i]->getSlot()) + " initialized successfully.");
        }
        result &= initResult;
    }

    //enable all outputs
    INFO_PRINTLN("[CANTramCore] Enabling all outputs...");
    for(int i=0;i<providedGPIOs;i++) {
        if(outputDefinitionTable[i] == nullptr) continue;

        //Call the enable function of the output provider
        outputDefinitionTable[i]->provider->enableOutputs();
    }
    INFO_PRINTLN("[CANTramCore] All outputs enabled.");
    return result;
}

/**
 * @brief Call this function to reset the CANTram core and all attached modules.
 * @details This function performs a complete reset of all CANTram related functions. It resets the internal state of the core, clears the module and hardware resource lists, and resets all attached modules and hardware resources. It should be called to reinitialize the core to a clean state. Normally this function should only be used during testing, but not in production code.
 * 
 * @return true if the reset was successful, false otherwise
 */
bool CANTramCore::reset() {
    // Reset internal state
    INFO_PRINTLN("[CANTramCore] Resetting core...");
    moduleCount = 0;
    providedGPIOs = 0;
    usedGPIOs = 0;
    INFO_PRINTLN("[CANTramCore] Deleting module list...");
    for(int i=0;i<MAX_MODULES;i++) {
        if (modules[i] == nullptr) continue;
        modules[i]->reset();
        modules[i] = nullptr;
    }
    INFO_PRINTLN("[CANTramCore] Deleting hardware resource list...");
    for(int i=0;i<MAX_HARDWARE_RESOURCES;i++) {
        if(hardwareResources[i] == nullptr) continue;
        hardwareResources[i]->reset();
        hardwareResources[i] = nullptr;
    }
    INFO_PRINTLN("[CANTramCore] Deleting output definition table...");
    for(int i=0;i<MAX_GPIO;i++) {
        outputDefinitionTable[i] = nullptr;
    }
    outputDefinitionTableInitialized = false;
    INFO_PRINTLN("[CANTramCore] INFO: Core reset successfully.");
    return true;
}