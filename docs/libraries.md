# CANTram Framework Libraries

This document provides an overview of all libraries included in the CANTram Framework.

## Core Communication Libraries

### CANCore
**Status**: ✅ Available  
**Description**: Fundamental CAN bus communication functionality  
**Features**: 
- Standard and extended CAN frames
- Message filtering
- Multiple baud rates (5kbps - 1Mbps)
- Statistics tracking

### CANProtocol
**Status**: 🚧 Planned  
**Description**: High-level CAN protocol implementations  
**Features**:
- CANopen support
- J1939 protocol
- Custom protocol definitions

### Modbus
**Status**: 🚧 Planned  
**Description**: Modbus RTU/TCP communication  
**Features**:
- Modbus RTU over serial
- Modbus TCP over Ethernet
- Master and slave modes

### Ethernet
**Status**: 🚧 Planned  
**Description**: Ethernet communication support  
**Features**:
- TCP/IP stack integration
- Industrial Ethernet protocols
- Web server capabilities

## Core Control Libraries

### PLCCore
**Status**: ✅ Available  
**Description**: Fundamental PLC functionality  
**Features**:
- Scan cycle management
- State machine control
- Timing and monitoring
- Watchdog timer

### Ladder
**Status**: 🚧 Planned  
**Description**: Ladder logic interpreter  
**Features**:
- IEC 61131-3 compliant
- Runtime ladder execution
- Symbolic addressing

### PID
**Status**: 🚧 Planned  
**Description**: PID controller implementation  
**Features**:
- Proportional-Integral-Derivative control
- Anti-windup
- Auto-tuning support
- Bumpless transfer

### Motion
**Status**: 🚧 Planned  
**Description**: Motion control library  
**Features**:
- Stepper motor control
- Servo motor control
- Trajectory planning
- Multi-axis coordination

## I/O Libraries

### DigitalIO
**Status**: ✅ Available  
**Description**: Digital input/output handling  
**Features**:
- Debouncing
- Edge detection
- Pulse generation
- Toggle functionality

### AnalogIO
**Status**: 🚧 Planned  
**Description**: Analog input/output with filtering  
**Features**:
- Scaling and linearization
- Moving average filtering
- Min/max tracking
- Alarm thresholds

### PWM
**Status**: 🚧 Planned  
**Description**: PWM generation and control  
**Features**:
- Multiple PWM channels
- Frequency control
- Duty cycle control
- Synchronized outputs

### Counter
**Status**: 🚧 Planned  
**Description**: High-speed counter functionality  
**Features**:
- Up/down counting
- Encoder support
- Frequency measurement
- Position tracking

## Utility Libraries

### Timer
**Status**: ✅ Available  
**Description**: Timers and scheduling utilities  
**Features**:
- TON (On-delay timer)
- TOF (Off-delay timer)
- TP (Pulse timer)
- Software timers

### Logger
**Status**: 🚧 Planned  
**Description**: Logging and debugging utilities  
**Features**:
- Multiple log levels
- Timestamping
- Multiple output targets
- Circular buffer

### Config
**Status**: 🚧 Planned  
**Description**: Configuration management  
**Features**:
- Parameter storage
- EEPROM/Flash persistence
- JSON configuration
- Default values

### Math
**Status**: 🚧 Planned  
**Description**: Mathematical utilities  
**Features**:
- Fixed-point arithmetic
- Lookup tables
- Interpolation
- Statistical functions

## Data Handling Libraries

### DataBuffer
**Status**: 🚧 Planned  
**Description**: Circular buffer implementation  
**Features**:
- FIFO/LIFO modes
- Thread-safe operations
- Overflow handling

### DataLogger
**Status**: 🚧 Planned  
**Description**: Data logging to storage  
**Features**:
- SD card logging
- Flash logging
- CSV format support
- Timestamp support

### Database
**Status**: 🚧 Planned  
**Description**: Simple database functionality  
**Features**:
- Tag database
- Trend logging
- Alarm history

## Display Libraries

### Display
**Status**: 🚧 Planned  
**Description**: Display driver abstraction  
**Features**:
- LCD support
- OLED support
- Touch screen support
- Graphics primitives

### HMI
**Status**: 🚧 Planned  
**Description**: Human-Machine Interface elements  
**Features**:
- Buttons and switches
- Indicators
- Trend displays
- Alarm display

## Safety Libraries

### SafetyIO
**Status**: 🚧 Planned  
**Description**: Safety-rated I/O handling  
**Features**:
- Redundant inputs
- Test pulse generation
- Discrepancy detection
- Diagnostic coverage

### Watchdog
**Status**: 🚧 Planned  
**Description**: Enhanced watchdog functionality  
**Features**:
- Multiple watchdog timers
- Window watchdog
- Heartbeat monitoring

## Diagnostic Libraries

### Diagnostics
**Status**: 🚧 Planned  
**Description**: System diagnostics and monitoring  
**Features**:
- CPU load monitoring
- Memory usage tracking
- I/O diagnostics
- Communication statistics

### OTA
**Status**: 🚧 Planned  
**Description**: Over-The-Air update support  
**Features**:
- Firmware updates
- Configuration updates
- Rollback support
- Secure updates

## Additional Planned Libraries

- **SCADA**: SCADA system integration
- **Recipe**: Recipe management
- **Alarm**: Alarm handling and management
- **Historian**: Data historian functionality
- **Security**: Authentication and encryption
- **FileSystem**: File system operations
- **RTC**: Real-time clock utilities
- **Statistics**: Statistical process control

## Legend

- ✅ Available - Library is implemented and available
- 🚧 Planned - Library is planned for future implementation
- 🔄 In Progress - Library is currently being developed
- ⚠️ Experimental - Library is experimental and may change

## Usage

To use a library in your project, include the corresponding header file:

```cpp
#include <CANCore.h>
#include <PLCCore.h>
#include <DigitalIO.h>
#include <Timer.h>
```

For PlatformIO projects, the libraries are automatically available when you include the CANTram Framework in your `lib_deps`.

## Contributing

If you would like to contribute a new library or improve an existing one, please see [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.
