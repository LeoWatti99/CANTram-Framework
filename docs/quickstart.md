# Quick Start Guide

Get started with the CANTram Framework in minutes!

## Installation

### Option 1: Using PlatformIO (Recommended)

1. **Install PlatformIO**
   - [Download VS Code](https://code.visualstudio.com/)
   - Install the PlatformIO extension

2. **Clone the Framework**
   ```bash
   git clone https://github.com/LeoWatti99/CANTram-Framework.git
   cd CANTram-Framework
   ```

3. **Open in PlatformIO**
   - Open VS Code
   - File → Open Folder → Select CANTram-Framework directory
   - PlatformIO will automatically configure the project

### Option 2: Using CMake

```bash
git clone https://github.com/LeoWatti99/CANTram-Framework.git
cd CANTram-Framework
mkdir build && cd build
cmake ..
make
```

## Your First Project

### Step 1: Choose an Example

The framework includes several examples:
- **simple_plc**: Basic PLC with digital I/O
- **can_communication**: CAN bus messaging

### Step 2: Configure Your Board

Edit `platformio.ini` to select your board:

```ini
[platformio]
default_envs = esp32dev  ; or teensy40, nucleo_f446re, etc.
```

### Step 3: Build and Upload

```bash
# Build
pio run

# Upload to board
pio run -t upload

# Monitor serial output
pio device monitor
```

## Creating Your Own Project

### Basic Template

```cpp
#include <Arduino.h>
#include <PLCCore.h>
#include <CANCore.h>

// Create your PLC class
class MyPLC : public PLCCore {
public:
    MyPLC() : PLCCore(10) {}  // 10ms cycle
    
protected:
    void scanInputs() override {
        // Read your inputs here
    }
    
    void executeLogic() override {
        // Your control logic here
    }
    
    void updateOutputs() override {
        // Write your outputs here
    }
};

MyPLC plc;

void setup() {
    Serial.begin(115200);
    plc.initialize();
    plc.start();
}

void loop() {
    plc.executeCycle();
}
```

## Common Hardware Setup

### ESP32 with CAN Transceiver

**Connections:**
```
ESP32         CAN Transceiver
GPIO4  -----> CAN_TX
GPIO5  -----> CAN_RX
3.3V   -----> VCC
GND    -----> GND

CAN Transceiver
CAN_H  -----> CAN Bus High
CAN_L  -----> CAN Bus Low
```

**Termination:**
- Add 120Ω resistor between CAN_H and CAN_L at both ends of the bus

### STM32 with Built-in CAN

```
STM32F407     Function
PA11   -----> CAN1_RX
PA12   -----> CAN1_TX
```

Connect to CAN transceiver (MCP2551, TJA1050, etc.)

### Teensy with FlexCAN

```
Teensy 3.6/4.0   Function
Pin 3   -------> CAN_TX
Pin 4   -------> CAN_RX
```

Connect to CAN transceiver

## Testing Your Setup

### 1. Test Serial Communication

```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("CANTram Framework Test");
}

void loop() {
    Serial.println("Running...");
    delay(1000);
}
```

Upload and check Serial Monitor at 115200 baud.

### 2. Test Digital I/O

```cpp
#include <DigitalIO.h>

DigitalOutput led(13);

void setup() {
    led.begin();
}

void loop() {
    led.toggle();
    delay(500);
}
```

LED should blink.

### 3. Test PLC Core

```cpp
#include <PLCCore.h>

PLCCore plc(10);

void setup() {
    Serial.begin(115200);
    plc.initialize();
    plc.start();
}

void loop() {
    plc.executeCycle();
    
    if ((plc.getCycleCount() % 100) == 0) {
        Serial.println("PLC running");
    }
}
```

Should print "PLC running" every second.

## Troubleshooting

### Build Errors

**"Library not found"**
- Ensure all libraries are in the `lib/` directory
- Check `library.json` files are present

**"Function not defined"**
- Include the correct header file
- Check namespace and class names

### Upload Errors

**"Port not found"**
- Check USB cable (must be data cable, not charge-only)
- Install USB drivers for your board
- Try a different USB port

**"Permission denied"**
- Linux: Add user to dialout group
  ```bash
  sudo usermod -a -G dialout $USER
  ```
- Logout and login again

### Runtime Errors

**CAN bus not working**
- Check transceiver connections
- Verify 120Ω termination resistors
- Check baud rate matches all nodes
- Verify CAN_H and CAN_L are not swapped

**Serial output garbage**
- Check baud rate is 115200
- Verify correct board selected in platformio.ini
- Try different USB cable

## Next Steps

1. **Read the Documentation**
   - [Library Reference](libraries.md)
   - [Development Guide](development.md)

2. **Explore Examples**
   - Study the example projects
   - Modify them for your needs
   - Create your own examples

3. **Build Your Application**
   - Design your control logic
   - Implement using framework libraries
   - Test thoroughly

4. **Join the Community**
   - Star the GitHub repository
   - Report issues you find
   - Contribute improvements

## Getting Help

- **Documentation**: Check [README.md](../README.md)
- **Examples**: Look at working examples in `examples/`
- **Issues**: Open a GitHub issue
- **Contributing**: See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [Arduino Reference](https://www.arduino.cc/reference/)
- [IEC 61131-3 PLC Standard](https://en.wikipedia.org/wiki/IEC_61131-3)

## Success!

Once you see your first "PLC running" message, you're ready to build industrial control systems with CANTram Framework! 🎉
