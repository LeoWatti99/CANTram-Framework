#ifndef ESP32PWMCORE_H
#define ESP32PWMCORE_H

#include <driver/ledc.h>
#include <PWMCore.h>
#include "OutputDefinition.h"
#include "Debug.h"
class ESP32PWMCore : public PWMCore {
    public:
        bool reset() override;
        void begin() override;
        bool addChannel(OutputDefinition* def) override;

        void setFrequency(uint32_t freqHz) {
            _PWMTimerConfig.freq_hz = freqHz;
        }
        void setResolution(ledc_timer_bit_t resolution) {
            _PWMTimerConfig.duty_resolution = resolution;
        }
        void overrideTimer(ledc_timer_t timer) {
            _PWMTimerConfig.timer_num = timer;
        }
        void overrideSpeedMode(ledc_mode_t mode) {
            _PWMTimerConfig.speed_mode = mode;
        }
        void setDuty(OutputDefinition* def, uint16_t duty) override;

    private:
        const uint8_t MAX_CHANNELS = 8;
        ledc_timer_config_t _PWMTimerConfig{
            .speed_mode       = LEDC_HIGH_SPEED_MODE,
            .duty_resolution  = LEDC_TIMER_12_BIT, // Set duty resolution to 12 bits
            .timer_num        = LEDC_TIMER_0,     // Use timer 0
            .freq_hz          = 5000,              // Set frequency to 5 kHz
            .clk_cfg          = LEDC_AUTO_CLK      // Auto select the source clock
        };
        uint8_t _ledcChannelCount;

        static ledc_channel_t _ledcChannels[8]; // Array to hold channel numbers

        static constexpr int MAX_GPIO = 40; // ESP32 has up to 39 GPIOs
        int8_t _channelLUT[MAX_GPIO]; // checks to which channel a pin is assigned

    protected:
        uint8_t getMaxUsages() override { return 8; } //PWMCore can only be used by one module (the main module)
        
};

#endif