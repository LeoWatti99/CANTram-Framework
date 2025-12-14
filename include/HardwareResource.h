#ifndef HARDWARE_RESOURCE_H
#define HARDWARE_RESOURCE_H

#include "Debug.h"

/**
 * @file HardwareResource.h
 * @brief Base class for hardware resources.
 * @details Defines the abstract base class for hardware resources (UART, SPI, I2C, PWM) used by the CANTramCore.
 *          Provides usage counting and basic lifecycle methods that concrete hardware resource implementations should override.
 */

/**
 * @brief Base class for hardware resources like UART, SPI, I2C, PWM, etc.
 * @details HardwareResource provides a common interface for resources managed by CANTramCore.
 *          It implements usage counting and default reset/usage behaviour. Derived classes must implement getType()
 *          and may override getMaxUsages() and reset() as required.
 */
class HardwareResource{
  public: 

  /**
   * @brief Enumeration of hardware resource types.
   * @details Used to identify the concrete resource type when requesting or attaching resources.
   */
  enum Type{
    UART, //!< UART type
    SPI,  //!< SPI type
    I2C,  //!< I2C type
    PWM,   //!< PWM type
    CAN    //!< CAN type
  }; 

  /**
   * @brief Get the hardware resource type.
   * @details Must be implemented by derived classes to return the correct Type enum value.
   * @return Type The type of the hardware resource.
   */
  virtual Type getType()=0;

  /**
   * @brief Reset the hardware resource to a known default state.
   * @details Default implementation clears the usage counter and logs the reset. Derived classes should call the base
   *          implementation and perform any additional hardware-specific reset steps.
   * @return true if the reset succeeded, false otherwise.
   */
  virtual bool reset(){
    DEBUG_PRINTLN("[HardwareResource] Resetting hardware resource of type " + String((int)getType()));
    usages = 0;
    return true;
  }

  /**
   * @brief Request a usage slot for this resource.
   * @details Increments the internal usage counter if the resource has remaining capacity. If the resource is already
   *          at its maximum usages, the request is rejected and an error is logged.
   * @return true if the usage request was granted, false if the resource is already fully used.
   */
  bool requestUsage(){
    if(usages >= getMaxUsages() && getMaxUsages() != 0){
      ERROR_PRINTLN("[HardwareResource] Hardware resource of type " + String((int)getType()) + " already used by maximum number of modules (" + String(getMaxUsages()) + ").");
      return false;
    }
    usages++;
    DEBUG_PRINTLN("[HardwareResource] Hardware resource of type " + String((int)getType()) + " usage requested. Current usages: " + String(usages) + "/" + String(getMaxUsages()));
    return true;
  }

  /**
   * @brief Get the current number of usages of this resource.
   * @details Returns the number of modules that have requested/are using this hardware resource.
   * @return uint8_t Current usage count.
   */
  uint8_t getUsages(){return usages;}

  /**
   * @brief Check if the resource is currently used.
   * @details Returns true when at least one usage has been requested.
   * @return true if the resource is in use, false otherwise.
   */
  bool isUsed(){ return usages > 0; }

  /**
   * @brief Get remaining usage capacity for this resource.
   * @details Computes remaining usages as max usages minus current usages. Can be overridden by derived classes if needed.
   * @return uint8_t Number of remaining usages available.
   */
  virtual uint8_t getRemainingUsages(){ return getMaxUsages() - usages; }

  private:
    
    
  protected: 
    /**
     * @brief Current usage counter.
     * @details Tracks how many clients/modules have requested this resource.
     */
    uint8_t usages=0;

    /**
     * @brief Default constructor.
     * @details Protected to prevent direct instantiation of the abstract base class.
     */
    HardwareResource() = default;

    /**
     * @brief Get the maximum simultaneous usages allowed for this resource.
     * @details Default implementation returns 1. Override in derived classes to permit multiple users.
     * @return uint8_t Maximum number of concurrent usages allowed.
     */
    virtual uint8_t getMaxUsages(){ return 1; }

};

#endif