#ifndef PWMCORE_H
#define PWMCORE_H

#include "OutputDefinition.h"
#include "Debug.h"
#include "CANTramCore.h"
#include "HardwareResource.h"

/**
 * @file PWMCore.h
 * @brief Abstract PWM core interface for CANTram.
 * @details Declares the PWMCore base class used to provide PWM functionality to modules.
 *          Platform-specific implementations should derive from this class and implement channel
 *          allocation and duty control. PWMCore is also a HardwareResource and participates in
 *          the CANTramCore hardware resource lifecycle.
 */

// Base Class for PWM functionality, platform specific implementations should derive from this
class PWMCore : public HardwareResource {
    public:

        /**
         * @brief Initialize PWM core resources.
         * @details Perform any platform-specific initialization required before PWM channels can be
         *          allocated or used. Implementations should set internal initialized state and
         *          configure timers/hardware as needed.
         * @return void
         */
        virtual void begin();

        /**
         * @brief Add/allocate a PWM channel for the given output definition.
         * @details Reserve any internal timer/channel resources required to drive PWM on the provided
         *          OutputDefinition. The implementation should validate that the OutputDefinition
         *          supports PWM and return true on success.
         * @param def Pointer to the OutputDefinition to register for PWM.
         * @return true if the channel was added/allocated successfully, false otherwise.
         */
        virtual bool addChannel(OutputDefinition* def)=0; //Returns true if channel was added successfully

        /**
         * @brief Set PWM duty cycle for a registered output.
         * @details Drive the output described by def with the specified duty value. Duty units are
         *          implementation-specific but are expected to be in the range 0..4095 for 12-bit resolution.
         * @param def Pointer to the OutputDefinition to set PWM on.
         * @param duty Duty value to apply (e.g., 0..4095 for 12-bit resolution).
         * @return void
         */
        virtual void setDuty(OutputDefinition* def, uint16_t duty)=0; //Duty cycle between 0 and 4095 for 12 bit resolution

        /**
         * @brief Get the hardware resource type.
         * @details Overrides HardwareResource::getType() to identify this resource as a PWM type.
         * @return HardwareResource::Type Type::PWM
         */
        HardwareResource::Type getType() override { return Type::PWM; }

    private:
         

    protected:
        /**
         * @brief Protected default constructor.
         * @details Prevent direct instantiation of the base class. Derived implementations should call
         *          this constructor from their constructors.
         */
        PWMCore() = default;

        /**
         * @brief Internal initialized flag.
         * @details Indicates whether begin() has been called and hardware is ready for PWM operations.
         */
        bool _initialized;

        /**
         * @brief Get the maximum simultaneous usages allowed for this resource.
         * @details PWMCore limits concurrent users by default to a single module (the main module).
         *          Override in derived classes only if multiple simultaneous users are supported.
         * @return uint8_t Maximum number of concurrent usages (default 1).
         */
        virtual uint8_t getMaxUsages() override { return 1; } //PWMCore can only be used by one module (the main module)
};

#endif