# CANCore Library

Core CAN bus communication library for the CANTram Framework.

## Description

CANCore provides fundamental CAN bus communication functionality for distributed PLC systems based on microcontrollers. It offers a unified API across different microcontroller platforms (ESP32, STM32, Teensy, Arduino Due, etc.).

## Features

- Support for standard (11-bit) and extended (29-bit) CAN identifiers
- Configurable baud rates from 5kbps to 1Mbps
- Message filtering capabilities
- Statistics tracking (TX/RX counts, errors)
- Platform-independent API
- Efficient message handling

## Installation

This library is part of the CANTram Framework. Include it in your PlatformIO project:

```ini
lib_deps = 
    CANTram-Framework
```

## Usage

### Basic Example

```cpp
#include <CANCore.h>

CANCore canBus(CAN_500KBPS);

void setup() {
    Serial.begin(115200);
    
    if (canBus.begin()) {
        Serial.println("CAN bus initialized");
    } else {
        Serial.println("CAN bus initialization failed");
    }
}

void loop() {
    // Send a message
    CANMessage txMsg;
    txMsg.id = 0x100;
    txMsg.len = 8;
    txMsg.extended = false;
    txMsg.rtr = false;
    for (int i = 0; i < 8; i++) {
        txMsg.data[i] = i;
    }
    
    if (canBus.sendMessage(&txMsg)) {
        Serial.println("Message sent");
    }
    
    // Receive messages
    CANMessage rxMsg;
    if (canBus.receiveMessage(&rxMsg)) {
        Serial.print("Received ID: 0x");
        Serial.print(rxMsg.id, HEX);
        Serial.print(" Data: ");
        for (int i = 0; i < rxMsg.len; i++) {
            Serial.print(rxMsg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    
    delay(100);
}
```

### Advanced Features

#### Setting Message Filters

```cpp
// Accept only messages with ID 0x100-0x1FF
canBus.setFilter(0x100, 0xFF00, false);
```

#### Getting Statistics

```cpp
uint32_t tx, rx, errors;
canBus.getStatistics(&tx, &rx, &errors);
Serial.printf("TX: %d, RX: %d, Errors: %d\n", tx, rx, errors);
```

## API Reference

### Constructor

- `CANCore(uint32_t baudrate = CAN_500KBPS)` - Create CAN bus instance

### Methods

- `bool begin()` - Initialize CAN bus
- `void end()` - Stop CAN bus communication
- `bool sendMessage(const CANMessage* msg)` - Send a CAN message
- `bool receiveMessage(CANMessage* msg)` - Receive a CAN message
- `uint8_t available()` - Check number of available messages
- `void processMessages()` - Process pending messages
- `void setFilter(uint32_t id, uint32_t mask, bool extended)` - Set receive filter
- `void clearFilters()` - Clear all filters
- `uint8_t getLastError()` - Get last error code
- `void getStatistics(...)` - Get bus statistics

## Supported Platforms

- ESP32
- STM32 (all families with CAN)
- Teensy 3.x, 4.x
- Arduino Due
- Any ARM Cortex-M with CAN peripheral

## Dependencies

Platform-specific CAN drivers are required and automatically included based on target platform.

## License

GPL-3.0

## Author

Leo Watti
