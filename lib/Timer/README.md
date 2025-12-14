# Timer Library

Timer and scheduling utilities for the CANTram Framework.

## Description

Timer provides IEC 61131-3 compliant timer functions and general-purpose software timers for non-blocking delays and timing operations.

## Features

- **Timer**: General-purpose software timer
- **TON**: On-delay timer (IEC 61131-3)
- **TOF**: Off-delay timer (IEC 61131-3)
- **TP**: Pulse timer (IEC 61131-3)
- Non-blocking operation
- Millisecond precision
- Platform-independent

## Installation

This library is part of the CANTram Framework. Include it in your project:

```cpp
#include <Timer.h>
```

## Usage

### Software Timer Example

```cpp
#include <Arduino.h>
#include <Timer.h>

Timer myTimer(1000);  // 1 second timer

void setup() {
    Serial.begin(115200);
    myTimer.start();
}

void loop() {
    if (myTimer.elapsed()) {
        Serial.println("Timer elapsed!");
        myTimer.reset();
    }
}
```

### TON (On-Delay Timer) Example

```cpp
#include <Arduino.h>
#include <Timer.h>

TON motorDelay(5000);  // 5 second delay
bool startButton = false;

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Read input
    startButton = digitalRead(2);
    
    // Execute timer
    motorDelay.execute(startButton);
    
    // Use output
    if (motorDelay.Q()) {
        Serial.println("Motor ON");
    } else {
        Serial.println("Motor OFF");
    }
    
    delay(100);
}
```

### TOF (Off-Delay Timer) Example

```cpp
#include <Timer.h>

TOF fanDelay(60000);  // 1 minute delay

void loop() {
    bool heaterOn = digitalRead(3);
    
    fanDelay.execute(heaterOn);
    
    // Fan stays on for 1 minute after heater turns off
    digitalWrite(4, fanDelay.Q());
    
    delay(100);
}
```

### TP (Pulse Timer) Example

```cpp
#include <Timer.h>

TP hornPulse(500);  // 500ms pulse

void loop() {
    bool hornButton = digitalRead(5);
    
    hornPulse.execute(hornButton);
    
    // Horn sounds for 500ms on each button press
    digitalWrite(6, hornPulse.Q());
    
    delay(10);
}
```

## API Reference

### Timer

**Constructor:**
- `Timer(uint32_t intervalMs = 1000)`

**Methods:**
- `void setInterval(uint32_t intervalMs)` - Set timer interval
- `void start()` - Start the timer
- `void stop()` - Stop the timer
- `void reset()` - Reset timer to start
- `bool elapsed()` - Check if interval elapsed
- `uint32_t remaining()` - Get remaining time in ms
- `bool isRunning()` - Check if timer is running

### TON (On-Delay Timer)

**Constructor:**
- `TON(uint32_t presetMs = 1000)`

**Methods:**
- `void setPreset(uint32_t presetMs)` - Set preset time
- `void execute(bool input)` - Execute timer logic (call regularly)
- `bool Q()` - Get output state
- `uint32_t ET()` - Get elapsed time in ms

**Function:**
Output goes high after input has been high for preset time.

**Timing Diagram:**
```
Input:  ___┌────────────┐___
Output: ___┌──────┐_____└───
        <--PT-->
```

### TOF (Off-Delay Timer)

**Constructor:**
- `TOF(uint32_t presetMs = 1000)`

**Methods:**
- `void setPreset(uint32_t presetMs)` - Set preset time
- `void execute(bool input)` - Execute timer logic (call regularly)
- `bool Q()` - Get output state
- `uint32_t ET()` - Get elapsed time in ms

**Function:**
Output stays high for preset time after input goes low.

**Timing Diagram:**
```
Input:  ┌──────┐___________
Output: ┌─────────┐________
            <--PT-->
```

### TP (Pulse Timer)

**Constructor:**
- `TP(uint32_t presetMs = 1000)`

**Methods:**
- `void setPreset(uint32_t presetMs)` - Set preset time
- `void execute(bool input)` - Execute timer logic (call regularly)
- `bool Q()` - Get output state
- `uint32_t ET()` - Get elapsed time in ms

**Function:**
Generates a pulse of preset duration on rising edge of input.

**Timing Diagram:**
```
Input:  ___┌──┐____________
Output: ___┌─────┐_________
           <-PT->
```

## IEC 61131-3 Compliance

The TON, TOF, and TP timers are designed to comply with IEC 61131-3 PLC programming standard.

## Performance

- Memory: ~40 bytes per timer instance
- Execution time: < 10 microseconds per execute() call
- Resolution: 1 millisecond

## License

GPL-3.0

## Author

Leo Watti
