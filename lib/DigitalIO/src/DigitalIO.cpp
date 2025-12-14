/**
 * @file DigitalIO.cpp
 * @brief Digital I/O implementation for CANTram Framework
 * @author Leo Watti
 */

#include "DigitalIO.h"

#if defined(ARDUINO)
    #include <Arduino.h>
    #define PINMODE(pin, mode) pinMode(pin, mode)
    #define DIGITALREAD(pin) digitalRead(pin)
    #define DIGITALWRITE(pin, val) digitalWrite(pin, val)
    #define GET_MILLIS() millis()
#else
    // Non-Arduino platform stubs
    #define PINMODE(pin, mode) ((void)0)
    #define DIGITALREAD(pin) (false)
    #define DIGITALWRITE(pin, val) ((void)0)
    #define GET_MILLIS() (0)
#endif

// DigitalInput implementation
DigitalInput::DigitalInput(uint8_t pin, bool pullup, uint16_t debounceMs)
    : _pin(pin),
      _pullup(pullup),
      _debounceMs(debounceMs),
      _state(false),
      _lastState(false),
      _rawState(false),
      _lastChangeTime(0),
      _changed(false),
      _risingEdge(false),
      _fallingEdge(false) {
}

void DigitalInput::begin() {
#if defined(ARDUINO)
    if (_pullup) {
        PINMODE(_pin, INPUT_PULLUP);
    } else {
        PINMODE(_pin, INPUT);
    }
#endif
    _rawState = DIGITALREAD(_pin);
    _state = _rawState;
    _lastState = _state;
    _lastChangeTime = GET_MILLIS();
}

bool DigitalInput::read() {
    return _state;
}

bool DigitalInput::readRaw() {
    return DIGITALREAD(_pin);
}

bool DigitalInput::isHigh() {
    return _state;
}

bool DigitalInput::isLow() {
    return !_state;
}

bool DigitalInput::changed() {
    return _changed;
}

bool DigitalInput::risingEdge() {
    return _risingEdge;
}

bool DigitalInput::fallingEdge() {
    return _fallingEdge;
}

void DigitalInput::update() {
    _changed = false;
    _risingEdge = false;
    _fallingEdge = false;
    
    // Read raw state
    _rawState = DIGITALREAD(_pin);
    
    // Debouncing logic
    uint32_t currentTime = GET_MILLIS();
    
    if (_rawState != _lastState) {
        _lastChangeTime = currentTime;
    }
    
    if ((currentTime - _lastChangeTime) >= _debounceMs) {
        if (_rawState != _state) {
            _state = _rawState;
            _changed = true;
            
            if (_state) {
                _risingEdge = true;
            } else {
                _fallingEdge = true;
            }
        }
    }
    
    _lastState = _rawState;
}

// DigitalOutput implementation
DigitalOutput::DigitalOutput(uint8_t pin, bool initialState)
    : _pin(pin),
      _state(initialState),
      _pulseEndTime(0) {
}

void DigitalOutput::begin() {
#if defined(ARDUINO)
    PINMODE(_pin, OUTPUT);
    DIGITALWRITE(_pin, _state);
#endif
}

void DigitalOutput::write(bool state) {
    _state = state;
    DIGITALWRITE(_pin, _state);
}

void DigitalOutput::high() {
    write(true);
}

void DigitalOutput::low() {
    write(false);
}

void DigitalOutput::toggle() {
    write(!_state);
}

bool DigitalOutput::read() {
    return _state;
}

void DigitalOutput::pulse(uint32_t durationMs) {
    high();
    _pulseEndTime = GET_MILLIS() + durationMs;
    
    // Note: Application must call update() regularly to end pulse
    // Or use a timer interrupt
}
