/**
 * @file PLCCore.cpp
 * @brief Core PLC functionality library implementation
 * @author Leo Watti
 */

#include "PLCCore.h"

#if defined(ARDUINO)
    #include <Arduino.h>
    #define GET_MICROS() micros()
    #define GET_MILLIS() millis()
#else
    #include <chrono>
    #define GET_MICROS() (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
    #define GET_MILLIS() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
#endif

PLCCore::PLCCore(uint32_t cycleTime)
    : _state(PLC_STOPPED),
      _currentPhase(SCAN_INPUT),
      _targetCycleTime(cycleTime * 1000), // Convert to microseconds
      _actualCycleTime(0),
      _maxCycleTime(0),
      _cycleCount(0),
      _cycleStartTime(0),
      _watchdogTimeout(0),
      _lastWatchdogReset(0),
      _cycleOverrun(false),
      _lastError(0),
      _initialized(false) {
}

PLCCore::~PLCCore() {
    stop();
}

bool PLCCore::initialize() {
    if (_initialized) {
        return true;
    }
    
    _state = PLC_STOPPED;
    _currentPhase = SCAN_INPUT;
    _cycleCount = 0;
    _actualCycleTime = 0;
    _maxCycleTime = 0;
    _cycleOverrun = false;
    _lastError = 0;
    _lastWatchdogReset = GET_MILLIS();
    
    _initialized = true;
    return true;
}

bool PLCCore::start() {
    if (!_initialized) {
        _lastError = 1;
        return false;
    }
    
    if (_state == PLC_RUNNING) {
        return true;
    }
    
    _state = PLC_STARTING;
    _cycleStartTime = GET_MICROS();
    _lastWatchdogReset = GET_MILLIS();
    resetStatistics();
    _state = PLC_RUNNING;
    
    return true;
}

void PLCCore::stop() {
    if (_state != PLC_STOPPED) {
        _state = PLC_STOPPING;
        // Perform any cleanup needed
        _state = PLC_STOPPED;
    }
}

bool PLCCore::executeCycle() {
    if (_state != PLC_RUNNING) {
        return false;
    }
    
    // Check watchdog
    if (!checkWatchdog()) {
        _state = PLC_ERROR;
        _lastError = 2;
        handleError(_lastError);
        return false;
    }
    
    // Record cycle start time
    _cycleStartTime = GET_MICROS();
    
    // Execute scan phases
    _currentPhase = SCAN_INPUT;
    scanInputs();
    
    _currentPhase = SCAN_LOGIC;
    executeLogic();
    
    _currentPhase = SCAN_OUTPUT;
    updateOutputs();
    
    _currentPhase = SCAN_COMMUNICATION;
    processCommunication();
    
    // Update cycle timing
    updateCycleTiming();
    
    // Increment cycle counter
    _cycleCount++;
    
    // Reset watchdog
    _lastWatchdogReset = GET_MILLIS();
    
    return true;
}

PLCState PLCCore::getState() const {
    return _state;
}

ScanPhase PLCCore::getScanPhase() const {
    return _currentPhase;
}

uint32_t PLCCore::getCycleTime() const {
    return _actualCycleTime;
}

uint32_t PLCCore::getMaxCycleTime() const {
    return _maxCycleTime;
}

uint32_t PLCCore::getCycleCount() const {
    return _cycleCount;
}

void PLCCore::resetStatistics() {
    _cycleCount = 0;
    _maxCycleTime = 0;
    _actualCycleTime = 0;
    _cycleOverrun = false;
}

void PLCCore::setWatchdogTimeout(uint32_t timeout) {
    _watchdogTimeout = timeout;
    _lastWatchdogReset = GET_MILLIS();
}

bool PLCCore::isCycleOverrun() const {
    return _cycleOverrun;
}

uint8_t PLCCore::getLastError() const {
    return _lastError;
}

// Protected virtual methods - can be overridden by derived classes
void PLCCore::scanInputs() {
    // Default implementation - override in derived class
}

void PLCCore::executeLogic() {
    // Default implementation - override in derived class
}

void PLCCore::updateOutputs() {
    // Default implementation - override in derived class
}

void PLCCore::processCommunication() {
    // Default implementation - override in derived class
}

void PLCCore::handleError(uint8_t errorCode) {
    // Default implementation - override in derived class
    (void)errorCode;
}

// Private methods
void PLCCore::updateCycleTiming() {
    uint32_t currentTime = GET_MICROS();
    _actualCycleTime = currentTime - _cycleStartTime;
    
    // Update max cycle time
    if (_actualCycleTime > _maxCycleTime) {
        _maxCycleTime = _actualCycleTime;
    }
    
    // Check for cycle overrun
    _cycleOverrun = (_actualCycleTime > _targetCycleTime);
    
    // Wait for remaining cycle time if we're ahead
    if (!_cycleOverrun && _targetCycleTime > _actualCycleTime) {
        uint32_t remainingTime = _targetCycleTime - _actualCycleTime;
        #if defined(ARDUINO)
            delayMicroseconds(remainingTime);
        #else
            // For non-Arduino platforms, simple busy wait
            uint32_t endTime = GET_MICROS() + remainingTime;
            while (GET_MICROS() < endTime) {
                // Busy wait
            }
        #endif
    }
}

bool PLCCore::checkWatchdog() {
    if (_watchdogTimeout == 0) {
        return true; // Watchdog disabled
    }
    
    uint32_t currentTime = GET_MILLIS();
    uint32_t elapsed = currentTime - _lastWatchdogReset;
    
    return (elapsed < _watchdogTimeout);
}
