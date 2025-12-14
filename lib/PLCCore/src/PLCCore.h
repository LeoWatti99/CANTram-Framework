/**
 * @file PLCCore.h
 * @brief Core PLC functionality library for CANTram Framework
 * @author Leo Watti
 * @version 1.0.0
 * @date 2024
 * 
 * This library provides fundamental PLC operations including scan cycle
 * management, state machine handling, and basic control logic.
 */

#ifndef PLCCORE_H
#define PLCCORE_H

#include <stdint.h>
#include <stdbool.h>

// PLC states
enum PLCState {
    PLC_STOPPED = 0,
    PLC_STARTING,
    PLC_RUNNING,
    PLC_STOPPING,
    PLC_ERROR
};

// PLC scan cycle phases
enum ScanPhase {
    SCAN_INPUT = 0,
    SCAN_LOGIC,
    SCAN_OUTPUT,
    SCAN_COMMUNICATION
};

/**
 * @class PLCCore
 * @brief Core PLC functionality class
 * 
 * Provides fundamental PLC operations including:
 * - Scan cycle management
 * - State machine control
 * - Timing and cycle control
 * - Error handling
 */
class PLCCore {
public:
    /**
     * @brief Constructor
     * @param cycleTime Target cycle time in milliseconds (default: 10ms)
     */
    PLCCore(uint32_t cycleTime = 10);
    
    /**
     * @brief Destructor
     */
    ~PLCCore();
    
    /**
     * @brief Initialize PLC
     * @return true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Start PLC execution
     * @return true if started successfully
     */
    bool start();
    
    /**
     * @brief Stop PLC execution
     */
    void stop();
    
    /**
     * @brief Execute one PLC scan cycle
     * Should be called in main loop
     * @return true if cycle completed successfully
     */
    bool executeCycle();
    
    /**
     * @brief Get current PLC state
     * @return Current PLCState
     */
    PLCState getState() const;
    
    /**
     * @brief Get current scan phase
     * @return Current ScanPhase
     */
    ScanPhase getScanPhase() const;
    
    /**
     * @brief Get actual cycle time in microseconds
     * @return Cycle time in microseconds
     */
    uint32_t getCycleTime() const;
    
    /**
     * @brief Get maximum cycle time recorded
     * @return Maximum cycle time in microseconds
     */
    uint32_t getMaxCycleTime() const;
    
    /**
     * @brief Get cycle counter
     * @return Number of completed cycles
     */
    uint32_t getCycleCount() const;
    
    /**
     * @brief Reset cycle statistics
     */
    void resetStatistics();
    
    /**
     * @brief Set watchdog timeout
     * @param timeout Timeout in milliseconds (0 = disabled)
     */
    void setWatchdogTimeout(uint32_t timeout);
    
    /**
     * @brief Check if cycle overrun occurred
     * @return true if last cycle exceeded target time
     */
    bool isCycleOverrun() const;
    
    /**
     * @brief Get last error code
     * @return Error code (0 = no error)
     */
    uint8_t getLastError() const;

protected:
    /**
     * @brief Input scan phase - override in derived class
     */
    virtual void scanInputs();
    
    /**
     * @brief Logic execution phase - override in derived class
     */
    virtual void executeLogic();
    
    /**
     * @brief Output update phase - override in derived class
     */
    virtual void updateOutputs();
    
    /**
     * @brief Communication phase - override in derived class
     */
    virtual void processCommunication();
    
    /**
     * @brief Error handler - override in derived class
     * @param errorCode Error code
     */
    virtual void handleError(uint8_t errorCode);

private:
    PLCState _state;
    ScanPhase _currentPhase;
    uint32_t _targetCycleTime;
    uint32_t _actualCycleTime;
    uint32_t _maxCycleTime;
    uint32_t _cycleCount;
    uint32_t _cycleStartTime;
    uint32_t _watchdogTimeout;
    uint32_t _lastWatchdogReset;
    bool _cycleOverrun;
    uint8_t _lastError;
    bool _initialized;
    
    // Internal methods
    void updateCycleTiming();
    bool checkWatchdog();
};

#endif // PLCCORE_H
