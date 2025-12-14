# PLCCore Library

Core PLC functionality library for the CANTram Framework.

## Description

PLCCore provides fundamental PLC operations including scan cycle management, state machine control, and basic control logic. It implements the standard PLC scan cycle pattern and provides timing control and monitoring.

## Features

- Standard PLC scan cycle (Input → Logic → Output → Communication)
- Configurable cycle time with overrun detection
- State machine management
- Watchdog timer support
- Cycle timing statistics
- Virtual methods for easy customization
- Platform-independent implementation

## Installation

This library is part of the CANTram Framework. Include it in your project:

```cpp
#include <PLCCore.h>
```

## Usage

### Basic Example

```cpp
#include <PLCCore.h>

// Create PLC with 10ms cycle time
PLCCore plc(10);

void setup() {
    Serial.begin(115200);
    
    if (plc.initialize()) {
        Serial.println("PLC initialized");
        plc.start();
    }
}

void loop() {
    plc.executeCycle();
    
    // Check for cycle overrun
    if (plc.isCycleOverrun()) {
        Serial.println("WARNING: Cycle overrun!");
    }
}
```

### Advanced Example with Custom Logic

```cpp
#include <PLCCore.h>

class MyPLC : public PLCCore {
public:
    MyPLC(uint32_t cycleTime) : PLCCore(cycleTime) {}

protected:
    void scanInputs() override {
        // Read inputs from hardware
        digitalInput1 = digitalRead(PIN_INPUT1);
        analogInput1 = analogRead(PIN_ANALOG1);
    }
    
    void executeLogic() override {
        // Your control logic here
        if (digitalInput1) {
            outputState = true;
        } else {
            outputState = false;
        }
    }
    
    void updateOutputs() override {
        // Write outputs to hardware
        digitalWrite(PIN_OUTPUT1, outputState);
    }
    
    void processCommunication() override {
        // Handle CAN bus or other communication
        // Send/receive data
    }

private:
    bool digitalInput1 = false;
    int analogInput1 = 0;
    bool outputState = false;
};

MyPLC plc(10);

void setup() {
    Serial.begin(115200);
    pinMode(PIN_INPUT1, INPUT);
    pinMode(PIN_OUTPUT1, OUTPUT);
    
    plc.initialize();
    plc.setWatchdogTimeout(1000); // 1 second watchdog
    plc.start();
}

void loop() {
    plc.executeCycle();
    
    // Print statistics every 1000 cycles
    if (plc.getCycleCount() % 1000 == 0) {
        Serial.printf("Cycle: %d, Time: %d us, Max: %d us\n",
                      plc.getCycleCount(),
                      plc.getCycleTime(),
                      plc.getMaxCycleTime());
    }
}
```

## API Reference

### Constructor

- `PLCCore(uint32_t cycleTime = 10)` - Create PLC instance with specified cycle time in ms

### Methods

- `bool initialize()` - Initialize PLC
- `bool start()` - Start PLC execution
- `void stop()` - Stop PLC execution
- `bool executeCycle()` - Execute one scan cycle
- `PLCState getState()` - Get current PLC state
- `ScanPhase getScanPhase()` - Get current scan phase
- `uint32_t getCycleTime()` - Get actual cycle time in microseconds
- `uint32_t getMaxCycleTime()` - Get maximum recorded cycle time
- `uint32_t getCycleCount()` - Get cycle counter
- `void resetStatistics()` - Reset timing statistics
- `void setWatchdogTimeout(uint32_t timeout)` - Set watchdog timeout in ms
- `bool isCycleOverrun()` - Check if cycle overrun occurred
- `uint8_t getLastError()` - Get last error code

### Virtual Methods (Override in Derived Class)

- `void scanInputs()` - Input reading phase
- `void executeLogic()` - Logic execution phase
- `void updateOutputs()` - Output writing phase
- `void processCommunication()` - Communication handling phase
- `void handleError(uint8_t errorCode)` - Error handling

## PLC States

- `PLC_STOPPED` - PLC is stopped
- `PLC_STARTING` - PLC is starting up
- `PLC_RUNNING` - PLC is running normally
- `PLC_STOPPING` - PLC is stopping
- `PLC_ERROR` - PLC encountered an error

## Scan Phases

- `SCAN_INPUT` - Reading inputs
- `SCAN_LOGIC` - Executing logic
- `SCAN_OUTPUT` - Updating outputs
- `SCAN_COMMUNICATION` - Processing communication

## License

GPL-3.0

## Author

Leo Watti
