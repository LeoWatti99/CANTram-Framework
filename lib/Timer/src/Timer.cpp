/**
 * @file Timer.cpp
 * @brief Timer implementation for CANTram Framework
 * @author Leo Watti
 */

#include "Timer.h"

#if defined(ARDUINO)
    #include <Arduino.h>
    #define GET_MILLIS() millis()
#else
    #include <chrono>
    #define GET_MILLIS() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
#endif

// Timer implementation
Timer::Timer(uint32_t intervalMs)
    : _interval(intervalMs),
      _startTime(0),
      _running(false) {
}

void Timer::setInterval(uint32_t intervalMs) {
    _interval = intervalMs;
}

void Timer::start() {
    _startTime = GET_MILLIS();
    _running = true;
}

void Timer::stop() {
    _running = false;
}

void Timer::reset() {
    _startTime = GET_MILLIS();
}

bool Timer::elapsed() {
    if (!_running) {
        return false;
    }
    
    uint32_t currentTime = GET_MILLIS();
    return ((currentTime - _startTime) >= _interval);
}

uint32_t Timer::remaining() {
    if (!_running) {
        return 0;
    }
    
    uint32_t currentTime = GET_MILLIS();
    uint32_t elapsed = currentTime - _startTime;
    
    if (elapsed >= _interval) {
        return 0;
    }
    
    return (_interval - elapsed);
}

bool Timer::isRunning() {
    return _running;
}

// TON (On-Delay Timer) implementation
TON::TON(uint32_t presetMs)
    : _preset(presetMs),
      _startTime(0),
      _input(false),
      _output(false),
      _timing(false) {
}

void TON::setPreset(uint32_t presetMs) {
    _preset = presetMs;
}

void TON::execute(bool input) {
    uint32_t currentTime = GET_MILLIS();
    
    // Rising edge on input
    if (input && !_input) {
        _startTime = currentTime;
        _timing = true;
    }
    
    // Falling edge on input
    if (!input && _input) {
        _timing = false;
        _output = false;
    }
    
    // Timer logic
    if (_timing && input) {
        uint32_t elapsed = currentTime - _startTime;
        if (elapsed >= _preset) {
            _output = true;
        }
    }
    
    _input = input;
}

bool TON::Q() {
    return _output;
}

uint32_t TON::ET() {
    if (!_timing) {
        return 0;
    }
    
    uint32_t currentTime = GET_MILLIS();
    uint32_t elapsed = currentTime - _startTime;
    
    return (elapsed > _preset) ? _preset : elapsed;
}

// TOF (Off-Delay Timer) implementation
TOF::TOF(uint32_t presetMs)
    : _preset(presetMs),
      _startTime(0),
      _input(false),
      _output(false),
      _timing(false) {
}

void TOF::setPreset(uint32_t presetMs) {
    _preset = presetMs;
}

void TOF::execute(bool input) {
    uint32_t currentTime = GET_MILLIS();
    
    // Input high - output follows
    if (input) {
        _output = true;
        _timing = false;
    }
    
    // Falling edge on input - start timing
    if (!input && _input) {
        _startTime = currentTime;
        _timing = true;
        _output = true;
    }
    
    // Timer logic
    if (_timing) {
        uint32_t elapsed = currentTime - _startTime;
        if (elapsed >= _preset) {
            _output = false;
            _timing = false;
        }
    }
    
    _input = input;
}

bool TOF::Q() {
    return _output;
}

uint32_t TOF::ET() {
    if (!_timing) {
        return 0;
    }
    
    uint32_t currentTime = GET_MILLIS();
    uint32_t elapsed = currentTime - _startTime;
    
    return (elapsed > _preset) ? _preset : elapsed;
}

// TP (Pulse Timer) implementation
TP::TP(uint32_t presetMs)
    : _preset(presetMs),
      _startTime(0),
      _lastInput(false),
      _output(false),
      _timing(false) {
}

void TP::setPreset(uint32_t presetMs) {
    _preset = presetMs;
}

void TP::execute(bool input) {
    uint32_t currentTime = GET_MILLIS();
    
    // Rising edge on input - start pulse
    if (input && !_lastInput && !_timing) {
        _startTime = currentTime;
        _timing = true;
        _output = true;
    }
    
    // Timer logic
    if (_timing) {
        uint32_t elapsed = currentTime - _startTime;
        if (elapsed >= _preset) {
            _output = false;
            _timing = false;
        }
    }
    
    _lastInput = input;
}

bool TP::Q() {
    return _output;
}

uint32_t TP::ET() {
    if (!_timing && !_output) {
        return 0;
    }
    
    uint32_t currentTime = GET_MILLIS();
    uint32_t elapsed = currentTime - _startTime;
    
    return (elapsed > _preset) ? _preset : elapsed;
}
