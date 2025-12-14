#include "CANTramModule.h"

/**
 * @brief Reset the module to a known default state.
 * @details Default implementation performs module reset actions. It logs the reset operation and returns true.
 *          Derived classes may override to perform additional hardware or state reset behavior.
 * @return true if reset succeeded, false otherwise.
 */
bool CANTramModule::reset() {
    DEBUG_PRINTLN("[CANTramModule] Resetting module in slot " + String(SLOT));
    return true;
} // Optional override to reset module state

/**
 * @brief Attach this module to a slot and assign its GPIO range.
 * @details Sets the module's SLOT and GPIO_START values, logs the attachment and runs the standard
 *          initialization sequence: provideGPIOs, provideHardwareResources, requestGPIOs,
 *          requestHardwareResources and addInterfaces. Logs success or warnings based on the combined result.
 * @param slot Slot index to attach the module to.
 * @param gpioStart Starting GPIO index assigned to this module.
 * @return true if all initialization steps succeeded, false if any step failed.
 */
bool CANTramModule::attachModule(uint8_t slot, uint8_t gpioStart) {
    SLOT = slot;
    GPIO_START = gpioStart;
    INFO_PRINTLN("[CANTramModule] Module attached at slot " + String(SLOT) + " with GPIO start index " + String(GPIO_START));
    bool result = true;
    //generic module configuration
    result &= provideGPIOs();
    result &= provideHardwareResources();
    result &= requestGPIOs();
    result &= requestHardwareResources();
    result &= addInterfaces();

    if(result) INFO_PRINTLN("[CANTramModule] Module in slot " + String(SLOT) + " successfully attached.");
    else WARNING_PRINTLN("[CANTramModule] WARNING: Module in slot " + String(SLOT) + " attachment encountered issues. Module might not function properly!");
    return result;
} // Pure virtual function to be implemented by derived classes. Initilazes hardware and registers modul