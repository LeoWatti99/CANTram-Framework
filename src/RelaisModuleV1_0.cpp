#include "RelaisModuleV1_0.h"
#include "CANTramCore.h"
#include "Debug.h"

const String RelaisModuleV1_0::HW_TYPE = "RelaisModule";
const String RelaisModuleV1_0::HW_VERSION = "1.0";
const String RelaisModuleV1_0::FW_VERSION = "1.0.0";

bool RelaisModuleV1_0::attachModule(uint8_t slot, uint8_t gpioStart){
  bool result = true;
    //general module setup
  result = CANTramModule::attachModule(slot, gpioStart); //Calls the generic module attach function
  
  return result;
}

bool RelaisModuleV1_0::provideGPIOs(){
   return true;
}

bool RelaisModuleV1_0::requestGPIOs(){
    bool result = true;
    _relaisGPIOs[0] = CANTramCore::getOutputDefinition(GPIO_START + 0);
    _relaisGPIOs[1] = CANTramCore::getOutputDefinition(GPIO_START + 2);
    _relaisGPIOs[2] = CANTramCore::getOutputDefinition(GPIO_START + 1);
    _relaisGPIOs[3] = CANTramCore::getOutputDefinition(GPIO_START + 3);
    for (int i=0;i<INTERFACE_COUNT;i++){
        if(_relaisGPIOs[i] == nullptr){
            WARNING_PRINTLN("[RelaisModuleV1_0] No output definition found at index " + String(GPIO_START + i) + " for interface " + _relaisInterfaces[i].getName() + ". Module cannot function without proper output definitions.");
            result = false;
            continue;
            
        }
        INFO_PRINTLN("[RelaisModuleV1_0] Requested output definition at index " + String(GPIO_START + i) + " for interface " + _relaisInterfaces[i].getName());
    }
    return result;
}

bool RelaisModuleV1_0::provideHardwareResources(){
    return true;
}

bool RelaisModuleV1_0::requestHardwareResources(){
    return true;
}

bool RelaisModuleV1_0::addInterfaces(){
    bool result=true;
    for(size_t i = 0; i < INTERFACE_COUNT; i++){
        _relaisInterfaces[i].rename("RQ" + String(i + 1));
        if(_relaisGPIOs[i] == nullptr){
            result = false;
            WARNING_PRINTLN("[RelaisModuleV1_0] Interface " + _relaisInterfaces[i].getName() + " not valid, no output definition assigned.");
        }
        _relaisInterfaces[i].validate();
        _interfaces[i] = &_relaisInterfaces[i];
        INFO_PRINTLN("[RelaisModuleV1_0]  Added interface " + _relaisInterfaces[i].getName() + " at index " + String(i) + ( _relaisInterfaces[i].isValid() ? " (valid)" : " (invalid, no output definition found)"));
    }
    return result;
}

bool RelaisModuleV1_0::initialize(){
    return true;
}

void RelaisModuleV1_0::cycle(uint8_t* response){
    CANTramModule::cycle(response); 
    for(size_t i = 0; i < INTERFACE_COUNT; i++){
        if(!_relaisInterfaces[i].isValid()){
            WARNING_PRINTLN("[RelaisModuleV1_0] Cannot update interface " + _relaisInterfaces[i].getName() + " because it is not valid.");
            continue;
        }

        bool desiredState = _relaisInterfaces[i].getQ();
        _relaisGPIOs[i]->provider->setOutput(_relaisGPIOs[i], desiredState);
        
    }
}

Interface** RelaisModuleV1_0::getInterfaces(){
    return _interfaces;
}