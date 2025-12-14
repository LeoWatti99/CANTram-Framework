/**
 * @file Timer.h
 * @brief Timer and scheduling utilities for CANTram Framework
 * @author Leo Watti
 * @version 1.0.0
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @class Timer
 * @brief Software timer for non-blocking delays
 */
class Timer {
public:
    Timer(uint32_t intervalMs = 1000);
    void setInterval(uint32_t intervalMs);
    void start();
    void stop();
    void reset();
    bool elapsed();
    uint32_t remaining();
    bool isRunning();

private:
    uint32_t _interval;
    uint32_t _startTime;
    bool _running;
};

/**
 * @class TON (Timer On-Delay)
 * @brief IEC 61131-3 compliant on-delay timer
 */
class TON {
public:
    TON(uint32_t presetMs = 1000);
    void setPreset(uint32_t presetMs);
    void execute(bool input);
    bool Q();
    uint32_t ET();

private:
    uint32_t _preset;
    uint32_t _startTime;
    bool _input;
    bool _output;
    bool _timing;
};

/**
 * @class TOF (Timer Off-Delay)
 * @brief IEC 61131-3 compliant off-delay timer
 */
class TOF {
public:
    TOF(uint32_t presetMs = 1000);
    void setPreset(uint32_t presetMs);
    void execute(bool input);
    bool Q();
    uint32_t ET();

private:
    uint32_t _preset;
    uint32_t _startTime;
    bool _input;
    bool _output;
    bool _timing;
};

/**
 * @class TP (Timer Pulse)
 * @brief IEC 61131-3 compliant pulse timer
 */
class TP {
public:
    TP(uint32_t presetMs = 1000);
    void setPreset(uint32_t presetMs);
    void execute(bool input);
    bool Q();
    uint32_t ET();

private:
    uint32_t _preset;
    uint32_t _startTime;
    bool _lastInput;
    bool _output;
    bool _timing;
};

#endif // TIMER_H
