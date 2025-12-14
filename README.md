# CANTram-Framework

A comprehensive C++ library collection for decentralized PLC systems based on microcontrollers. This framework provides modular libraries for building robust industrial control systems using CAN bus communication.

## Overview

CANTram-Framework is designed for distributed control systems where multiple microcontrollers communicate via CAN bus to perform PLC (Programmable Logic Controller) functions. The framework consists of approximately 30 specialized libraries, each handling specific aspects of industrial automation and control.

## Features

- **Modular Architecture**: Each library is independent and can be used standalone or combined
- **Microcontroller Optimized**: Designed for resource-constrained embedded systems
- **CAN Bus Communication**: Built-in support for industrial CAN protocols
- **Real-time Capable**: Suitable for time-critical control applications
- **Easy Integration**: Simple API design for quick project development

## Directory Structure

```
CANTram-Framework/
├── lib/                    # Individual library modules
│   ├── CANCore/           # Core CAN communication library
│   ├── PLCCore/           # Basic PLC functionality
│   ├── IOModule/          # Input/Output handling
│   ├── Timer/             # Timer and scheduling utilities
│   └── ...                # Additional libraries
├── examples/              # Example projects and usage demos
├── docs/                  # Documentation and guides
├── tools/                 # Utility scripts and tools
└── README.md             # This file
```

## Getting Started

### Prerequisites

- **PlatformIO**: Recommended for building and uploading to microcontrollers
- **CMake** (optional): For building on host systems
- **Compatible Microcontroller**: ARM Cortex-M, ESP32, STM32, or similar

### Installation

#### Using PlatformIO

1. Clone this repository:
   ```bash
   git clone https://github.com/LeoWatti99/CANTram-Framework.git
   cd CANTram-Framework
   ```

2. Include desired libraries in your `platformio.ini`:
   ```ini
   [env:your_board]
   platform = ...
   board = ...
   lib_deps = 
       CANTram-Framework
   ```

#### Using CMake

1. Clone and include in your CMakeLists.txt:
   ```cmake
   add_subdirectory(CANTram-Framework)
   target_link_libraries(your_target CANTram::Core)
   ```

### Quick Example

```cpp
#include <CANCore.h>
#include <PLCCore.h>

CANCore canBus(CAN_500KBPS);
PLCCore plc;

void setup() {
    canBus.begin();
    plc.initialize();
}

void loop() {
    canBus.processMessages();
    plc.executeCycle();
}
```

## Library Modules

The framework includes the following library categories:

### Communication
- **CANCore**: Basic CAN bus communication
- **CANProtocol**: High-level CAN protocol implementation
- **Modbus**: Modbus RTU/TCP support

### Control
- **PLCCore**: Core PLC functionality
- **Ladder**: Ladder logic interpreter
- **PID**: PID controller implementation

### I/O
- **DigitalIO**: Digital input/output handling
- **AnalogIO**: Analog input/output with filtering
- **PWM**: PWM generation and control

### Utilities
- **Timer**: Timers and scheduling
- **Logger**: Logging and debugging utilities
- **Config**: Configuration management

*See [docs/libraries.md](docs/libraries.md) for complete library documentation.*

## Development

### Creating a New Library

Each library follows a standard structure:

```
lib/YourLibrary/
├── src/
│   ├── YourLibrary.h
│   └── YourLibrary.cpp
├── examples/
│   └── basic_usage.cpp
├── README.md
└── library.json
```

See [docs/development.md](docs/development.md) for detailed development guidelines.

### Building

Using PlatformIO:
```bash
pio run
```

Using CMake:
```bash
mkdir build && cd build
cmake ..
make
```

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## Supported Platforms

- **Arduino**: ESP32, Due, Mega
- **STM32**: All STM32 families with CAN support
- **Teensy**: Teensy 3.x, 4.x with CAN
- **ARM mbed**: mbed-enabled boards
- **Custom**: Any ARM Cortex-M with CAN peripheral

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Authors

- **Leo Watti** - *Initial work* - [LeoWatti99](https://github.com/LeoWatti99)

## Acknowledgments

- Inspired by industrial automation requirements
- Built for the maker and professional community
- Special thanks to all contributors

## Support

For questions, issues, or feature requests, please open an issue on GitHub:
https://github.com/LeoWatti99/CANTram-Framework/issues

## Roadmap

- [ ] Complete all 30 core libraries
- [ ] Add comprehensive examples for each library
- [ ] Create web-based configuration tool
- [ ] Implement OTA (Over-The-Air) updates
- [ ] Add safety-critical library variants (SIL certified)
- [ ] Develop CANopen protocol support
- [ ] Create SCADA integration tools