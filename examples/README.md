# CANTram Framework Examples

This directory contains example projects demonstrating various features of the CANTram Framework.

## Available Examples

### 1. Simple PLC
**Path**: `simple_plc/`  
**Difficulty**: Beginner  
**Description**: Basic PLC application with digital I/O and timers

**Features Demonstrated**:
- PLC scan cycle management
- Digital input/output handling
- Input debouncing
- Timer operations (TON - On-delay timer)
- Basic control logic

**Hardware Requirements**:
- Any supported microcontroller
- 2 push buttons (Start/Stop)
- 2 LEDs (Motor/Status)

### 2. CAN Communication
**Path**: `can_communication/`  
**Difficulty**: Intermediate  
**Description**: CAN bus communication between distributed nodes

**Features Demonstrated**:
- CAN bus initialization and configuration
- Sending and receiving CAN messages
- Message filtering
- Heartbeat mechanism
- Status monitoring
- Statistics tracking

**Hardware Requirements**:
- Microcontroller with CAN support (ESP32, STM32, etc.)
- CAN transceiver (MCP2551, TJA1050, etc.)
- CAN bus connection to other nodes

### 3. Complete System (Coming Soon)
**Path**: `complete_system/`  
**Difficulty**: Advanced  
**Description**: Complete distributed PLC system with multiple nodes

**Features Demonstrated**:
- Multiple library integration
- Distributed control architecture
- Master/slave configuration
- Data sharing via CAN bus
- Alarm handling
- Configuration management

## Running Examples

### Using PlatformIO

1. Navigate to the example directory:
   ```bash
   cd examples/simple_plc
   ```

2. Build and upload:
   ```bash
   pio run -t upload
   ```

3. Monitor serial output:
   ```bash
   pio device monitor
   ```

### Using Arduino IDE

1. Open the `main.cpp` file (rename to `.ino` if needed)
2. Select your board and port
3. Click Upload
4. Open Serial Monitor at 115200 baud

### Using CMake

```bash
mkdir build && cd build
cmake .. -DCANTRAM_BUILD_EXAMPLES=ON
make
```

## Modifying Examples

### Changing Target Board

Edit `platformio.ini` in the root directory and select your environment:

```ini
[platformio]
default_envs = esp32dev  ; Change to your board
```

### Adjusting Pin Assignments

Pin definitions are at the top of each example's `main.cpp`:

```cpp
#define PIN_START_BTN   2   // Change to your pin
#define PIN_STOP_BTN    3   // Change to your pin
```

### Modifying Cycle Time

Change the PLC cycle time in the constructor:

```cpp
PLCCore plc(10);  // 10ms cycle time - adjust as needed
```

### CAN Bus Speed

Modify the CAN baud rate:

```cpp
CANCore canBus(CAN_500KBPS);  // Options: 125k, 250k, 500k, 1000k
```

## Troubleshooting

### Example Won't Compile

- Check that all required libraries are in the `lib/` directory
- Verify your PlatformIO or Arduino IDE is up to date
- Ensure the correct board is selected

### CAN Bus Not Working

- Verify CAN transceiver connections
- Check that CAN_H and CAN_L are properly terminated (120Ω resistors)
- Confirm the baud rate matches across all nodes
- Test with a CAN bus analyzer if available

### Serial Output Not Appearing

- Check baud rate is set to 115200
- Verify USB cable supports data (not charge-only)
- Try a different USB port
- Check driver installation for your board

## Creating Your Own Examples

To create a new example:

1. Create a new directory in `examples/`
2. Add a `main.cpp` file with your code
3. Include required headers from the framework
4. Document hardware requirements
5. Add a README.md explaining the example

Example template:

```cpp
#include <Arduino.h>
#include <CANCore.h>
#include <PLCCore.h>

// Your code here

void setup() {
    Serial.begin(115200);
    // Initialization
}

void loop() {
    // Main logic
}
```

## Learning Path

**Recommended order for beginners**:
1. Start with `simple_plc` to understand basic PLC concepts
2. Move to `can_communication` to learn bus communication
3. Study `complete_system` for advanced integration

## Additional Resources

- [Main Documentation](../README.md)
- [Library Reference](../docs/libraries.md)
- [Development Guide](../docs/development.md)
- [Contributing Guidelines](../CONTRIBUTING.md)

## Getting Help

- Check the main README for common issues
- Review library documentation
- Open an issue on GitHub
- Join community discussions

## License

All examples are licensed under GPL-3.0, same as the main framework.
