/**
 * @file DigitalIO.h
 * @brief Digital I/O handling library for CANTram Framework
 * @author Leo Watti
 * @version 1.0.0
 */

#ifndef DIGITALIO_H
#define DIGITALIO_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @class DigitalInput
 * @brief Digital input handler with debouncing
 */
class DigitalInput {
public:
    DigitalInput(uint8_t pin, bool pullup = true, uint16_t debounceMs = 50);
    void begin();
    bool read();
    bool readRaw();
    bool isHigh();
    bool isLow();
    bool changed();
    bool risingEdge();
    bool fallingEdge();
    void update();

private:
    uint8_t _pin;
    bool _pullup;
    uint16_t _debounceMs;
    bool _state;
    bool _lastState;
    bool _rawState;
    uint32_t _lastChangeTime;
    bool _changed;
    bool _risingEdge;
    bool _fallingEdge;
};

/**
 * @class DigitalOutput
 * @brief Digital output handler
 */
class DigitalOutput {
public:
    DigitalOutput(uint8_t pin, bool initialState = false);
    void begin();
    void write(bool state);
    void high();
    void low();
    void toggle();
    bool read();
    void pulse(uint32_t durationMs);
    void update();

private:
    uint8_t _pin;
    bool _state;
    uint32_t _pulseEndTime;
};

#endif // DIGITALIO_H
