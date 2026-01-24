#ifndef CANTRAMMODULE_H
#define CANTRAMMODULE_H

#include <Arduino.h>
#include "Interface.h"
#include "Debug.h"

/**
 * @file CANTramModule.h
 * @brief Base class for CANTram hardware modules.
 * @details Declares the abstract CANTramModule class which all hardware modules must implement.
 *          Provides lifecycle hooks for attachment, GPIO/resource provisioning, initialization and periodic cycling.
 */
class CANTramModule {
    public:
        /**
         * @brief Default constructor.
         * @details Constructs an empty CANTramModule instance. Derived classes should perform initialization in attachModule()/initialize().
         */
        CANTramModule() = default;

        /**
         * @brief Destructor.
         * @details Virtual destructor to ensure proper cleanup in derived classes.
         */
        virtual ~CANTramModule() = default;
        
        /**
         * @brief Get the hardware type string.
         * @details Returns a human readable hardware type identifier for the concrete module implementation.
         * @return String Hardware type identifier (e.g. "MainModuleV1.0").
         */
        virtual String getHWType() const = 0;

        /**
         * @brief Get the hardware version string.
         * @details Returns the hardware revision/version string for the module.
         * @return String Hardware version (e.g. "1.0").
         */
        virtual String getHWVersion() const = 0;

        /**
         * @brief Get the firmware version string.
         * @details Returns the firmware version string implemented by the module.
         * @return String Firmware version (e.g. "1.0").
         */
        virtual String getFWVersion() const = 0;
        
        /**
         * @brief Get the GPIO demand of the module.
         * @details Returns how many GPIOs this module requires from the core.
         * @return uint8_t Number of GPIOs demanded by the module.
         */
        virtual uint8_t getGPIODemand() const = 0;

        /**
         * @brief Get the GPIO supply capacity of the module.
         * @details Returns how many GPIOs this module provides/supplies to the system.
         * @return uint8_t Number of GPIOs supplied by the module.
         */
        virtual uint8_t getGPIOSupply() const = 0;

        /**
         * @brief Reset the module to a known default state.
         * @details Optional override for module-specific reset logic. Base implementation should be safe to call.
         * @return true if reset succeeded, false otherwise.
         */
        virtual bool reset();

        /**
         * @brief Get assigned slot index.
         * @details Returns the slot index the module was attached to via attachModule().
         * @return uint8_t Slot index.
         */
        uint8_t getSlot() const { return SLOT; }

        /**
         * @brief Get starting GPIO index assigned to the module.
         * @details Returns the first GPIO index allocated to this module.
         * @return uint8_t GPIO start index.
         */
        uint8_t getGPIOStart() const { return GPIO_START; }

        /**
         * @brief Periodic cycle callback.
         * @details Called regularly by the core to allow the module to perform time-based tasks.
         * @param response Optional pointer to a response buffer where the module may write response bytes. May be nullptr.
         */
        virtual void cycle(uint8_t* response){
            DEBUG_PRINTLN("[CANTramModule] cycle() called on module " + getHWType() + " in slot " + String(SLOT));
        }
        
        /**
         * @brief Attach this module to a slot and assign GPIO range.
         * @details Initializes the module's internal SLOT and GPIO_START and triggers GPIO/resource provisioning
         *          and interface registration sequences.
         * @param slot Slot index to attach the module to.
         * @param gpioStart Starting GPIO index assigned to this module.
         * @return true if attachment and provisioning succeeded, false otherwise.
         */
        virtual bool attachModule(uint8_t slot, uint8_t gpioStart);

        /**
         * @brief Provide GPIO definitions to the core.
         * @details Called during module attachment to allow the module to declare any GPIOs it owns or provides.
         * @return true if GPIO definitions were provided successfully, false otherwise.
         */
        virtual bool provideGPIOs() = 0;

        /**
         * @brief Request GPIO definitions from the core.
         * @details Called after provideGPIOs to obtain runtime GPIO assignments from CANTramCore.
         * @return true if required GPIOs were acquired and validated, false otherwise.
         */
        virtual bool requestGPIOs() = 0;

        /**
         * @brief Provide hardware resources (e.g., I2C, SPI, UART) to the core.
         * @details Called during attachment to attach HardwareResource instances that other modules can request.
         * @return true if resources were successfully provided, false otherwise.
         */
        virtual bool provideHardwareResources() = 0;

        /**
         * @brief Request hardware resources from the core.
         * @details Called when the module should acquire hardware resources supplied by the system (e.g., UART core).
         * @return true if all required hardware resources were granted, false otherwise.
         */
        virtual bool requestHardwareResources() = 0;

        /**
         * @brief Setup and validate module interfaces.
         * @details Populate the module's Interface* array, name interfaces and validate them as needed.
         * @return true if interfaces were successfully added and validated, false otherwise.
         */
        virtual bool addInterfaces() = 0;

        /**
         * @brief Pre-initialize the module after attachment and provisioning.
         * @details Perform any pre-initialization steps before the main initialize() call. May be used to establish e.g. i2c communication needed for initialization.
         * @return true if pre-initialization succeeded, false otherwise.
         */
        virtual bool preInitialize() { return true; }
        /**
         * @brief Initialize the module after attachment and provisioning.
         * @details Perform runtime initialization (start drivers, begin peripherals) once all modules have been attached.
         * @return true if initialization succeeded, false otherwise.
         */
        virtual bool initialize() = 0;
        
        /**
         * @brief Get pointer to module's interface array.
         * @details Returns a pointer to the internal Interface* array used by the core to enumerate interfaces.
         * @return Interface** Pointer to the first element of the module's interfaces array.
         */
        virtual Interface** getInterfaces() = 0;

        /**
         * @brief Get number of interfaces provided by the module.
         * @details Return the number of entries in the interfaces array returned by getInterfaces().
         * @return size_t Number of interface pointers.
         */
        virtual size_t getInterfaceCount() = 0;

    protected:
        uint8_t SLOT = 0;
        uint8_t GPIO_START = 0;
};

#endif
