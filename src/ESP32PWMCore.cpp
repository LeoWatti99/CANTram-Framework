#include "ESP32PWMCore.h"
#include <Arduino.h>
#include "CANTramCore.h"
#include "OutputDefinition.h"
#include "Debug.h"


ledc_channel_t ESP32PWMCore::_ledcChannels[8] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_CHANNEL_4, LEDC_CHANNEL_5, LEDC_CHANNEL_6, LEDC_CHANNEL_7};

void ESP32PWMCore::begin() {
    //Initialize PWM functionality here if needed
    if(PWMCore::_initialized) return;
   
    //Set up the timer
    ledc_timer_config(&_PWMTimerConfig);
    _initialized = true;
    INFO_PRINTLN("[ESP32PWMCore] INFO: Initialized with frequency " + String(_PWMTimerConfig.freq_hz) + " Hz, resolution " + String(_PWMTimerConfig.duty_resolution) + " bits, timer " + String(_PWMTimerConfig.timer_num) + ", speed mode " + String(_PWMTimerConfig.speed_mode));

    
}

bool ESP32PWMCore::addChannel(OutputDefinition* def) {
    uint8_t channelIndex = _ledcChannelCount++;
    if(!PWMCore::_initialized) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: PWMCore not initialized. Call PWMCore::begin() first.");
        return false;
    }
    if(channelIndex >= MAX_CHANNELS) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Maximum number of PWM channels reached.");
        return false;
    }
    if(!def->pwm) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: OutputDefinition does not specify a PWM output.");
        return false;
    }
    if(!HardwareResource::requestUsage()){
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Hardware resource busy, cannot add PWM channel.");
        return false;
    }

    ledc_channel_config_t ledcChannelConfig;
        ledcChannelConfig.channel    = _ledcChannels[channelIndex];
        ledcChannelConfig.duty       = 0; // Set duty to 0%
        ledcChannelConfig.gpio_num   = def->pinOrBit;
        ledcChannelConfig.speed_mode = _PWMTimerConfig.speed_mode;
        ledcChannelConfig.hpoint     = 0;
        ledcChannelConfig.timer_sel  = _PWMTimerConfig.timer_num;
        ledcChannelConfig.intr_type  = LEDC_INTR_DISABLE; // Add this line
    #if ESP_IDF_VERSION_MAJOR >= 4
        ledcChannelConfig.flags.output_invert = 0; // Add this line for ESP-IDF v4+
    #endif

    if(ledc_channel_config(&ledcChannelConfig) != ESP_OK) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Failed to configure PWM channel on GPIO " + String(def->pinOrBit));
        return false;
    }
    _channelLUT[def->pinOrBit] = channelIndex; // Map pin to channel
    INFO_PRINTLN("[ESP32PWMCore] INFO: Added PWM channel on GPIO " + String(def->pinOrBit) + " using channel " + String(channelIndex));
    return true;
}

void ESP32PWMCore::setDuty(OutputDefinition* def, uint16_t duty) {
    //Check for valid channel
    int channel = _channelLUT[def->pinOrBit];
    if(channel < 0 || channel >= _ledcChannelCount) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Invalid channel number " + String(channel));
        return;
    }
    //Check for valid duty cycle
    if(duty > ((1 << _PWMTimerConfig.duty_resolution) - 1)) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Invalid duty cycle " + String(duty) + " for resolution " + String(_PWMTimerConfig.duty_resolution) + " bits");
        return;
    }
    //Apply the duty cycle
    if(ledc_set_duty(_PWMTimerConfig.speed_mode, _ledcChannels[channel], duty) != ESP_OK) {
        ERROR_PRINTLN("[ESP32PWMCore] ERROR: Failed to set duty cycle on channel " + String(channel));
        return;
    }
    ledc_update_duty(_PWMTimerConfig.speed_mode, _ledcChannels[channel]);
    
}

bool ESP32PWMCore::reset() {
    if(!HardwareResource::reset()){
        return false;
    }
    if(!PWMCore::_initialized) {
        return true; //Nothing to reset
    }
    //Reset all channels
    for(int i=0;i<_ledcChannelCount;i++) {
        ledc_stop(_PWMTimerConfig.speed_mode, _ledcChannels[i], 0); // Stop channel and set GPIO to low
        _channelLUT[i] = -1; // Clear channel mapping
    }
    _ledcChannelCount = 0;
    PWMCore::_initialized = false;
    INFO_PRINTLN("[ESP32PWMCore] INFO: Reset complete.");
    return true;
}