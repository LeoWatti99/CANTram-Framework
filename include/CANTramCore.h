/**
 * @file CANTramCore.h
 * @author your name (you@domain.com)
 * @brief This class serves as the core manager for the CANTram modular system. It handles module attachment, GPIO management, hardware resource allocation and cyclic updates of the modules.
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CANTRAMCORE_H
#define CANTRAMCORE_H

#include <Arduino.h>
#include <CANTramModule.h>
#include "OutputDefinition.h"
#include "Debug.h"
#include "HardwareResource.h"
#include "CANCore.h"

#ifndef DEFAULT_REF
#define DEFAULT_REF -1 
#endif

typedef enum CANTramCoreError
{
  CAN_TRAM_OK,
  MAX_GPIOS_EXCEEDED,
  GPIO_OVERWRITE,
} CANTramCoreError;

/**
 * @brief This static class serves as the core manager for the CANTram modular system. It handles module attachment, GPIO management, hardware resource allocation and cyclic updates of the modules.
 * 
 */
class CANTramCore
{
public:
  static void setOutput(uint8_t nr, bool state);

  CANTramCore() = delete;  // Prevent instantiation
  ~CANTramCore() = delete; // Prevent destruction
  static bool attachModule(CANTramModule *module, int8_t index,int8_t gpioStart);
  static bool attachModule(CANTramModule *module);
  static CANTramModule* getModule(uint8_t index){
    if(index >= MAX_MODULES){
      return nullptr;
    }
    return modules[index];
  }
  // static void setOutputDefinitionTable(OutputDefinition* table, uint8_t size);
  static CANTramCoreError addOutputDefinitionPointers(OutputDefinition *def[], uint8_t size);
  static CANTramCoreError addOutputDefinitions(OutputDefinition def[], uint8_t size);
  static CANTramCoreError addOutputDefinition(OutputDefinition *def, uint8_t index, bool overwrite = true);
  static CANTramCoreError addOutputDefinition(OutputDefinition *def);
  static OutputDefinition *getOutputDefinition(uint8_t index);
  static OutputDefinition *useOutputDefinition(uint8_t index);
  static bool reset();
  static bool initialize();

  static bool attachHardwareResource(HardwareResource *resource);
  static HardwareResource *useHardwareResource(HardwareResource::Type type)
  
  {
    for (int i = 0; i < 30; i++)
    {
      if (hardwareResources[i] == nullptr)
        continue;
      if (hardwareResources[i]->getType() != type)
        continue;
      if (!hardwareResources[i]->requestUsage())
        continue;
      return hardwareResources[i];
    }
    WARNING_PRINTLN("[CANTramCore] WARNING: No hardware resource of type " + String((int)type) + " available.");
    return nullptr;
  }
  static uint8_t getUsedGPIOs() { return usedGPIOs; }
  static uint8_t getProvidedGPIOs() { return providedGPIOs; }

 
  static CANCore::Baudrate DEFAULT_CAN_BAUDRATE;

  static uint8_t loop();

private:
  static constexpr uint8_t MAX_MODULES = 20;
  static CANTramModule *modules[];
  static constexpr uint8_t MAX_HARDWARE_RESOURCES = 30;
  static HardwareResource *hardwareResources[];
  static uint8_t moduleCount;
  static uint8_t providedGPIOs;            // Tells the core how many GPIOs are provided
  static uint8_t usedGPIOs;                // Tells the core how many GPIOs are used by modules
  static constexpr uint8_t MAX_GPIO = 100; // Maximum number of GPIOs (including shift register outputs)
  static OutputDefinition *outputDefinitionTable[];

  static bool outputDefinitionTableInitialized;

  // Private members here
};

#endif