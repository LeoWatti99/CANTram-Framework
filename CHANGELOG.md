# Changelog

All notable changes to the CANTram Framework will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Complete implementation of all 30 planned libraries
- Add comprehensive test suite
- Web-based configuration tool
- OTA update support
- Safety library variants (SIL certified)
- CANopen protocol support
- SCADA integration tools

## [1.0.0] - 2024-12-14

### Added

#### Framework Structure
- Initial repository structure with `lib/`, `examples/`, `docs/`, and `tools/` directories
- Comprehensive README with project overview and features
- Development guidelines and contributing documentation
- Quick start guide for new users
- Library reference documentation

#### Build System
- PlatformIO configuration supporting multiple platforms:
  - ESP32 (esp32dev, esp32-s3-devkitc-1)
  - STM32 (nucleo_f446re, bluepill_f103c8, black_f407ve)
  - Teensy (teensy36, teensy40)
  - Arduino Due
- CMake build system for desktop/embedded builds
- Proper .gitignore for C++/embedded projects

#### Core Libraries

##### CANCore (v1.0.0)
- CAN bus communication foundation
- Support for standard (11-bit) and extended (29-bit) identifiers
- Configurable baud rates (5kbps - 1Mbps)
- Message filtering capabilities
- Statistics tracking (TX/RX/errors)
- Platform-independent API with platform-specific implementations

##### PLCCore (v1.0.0)
- Standard PLC scan cycle implementation
- State machine management (Stopped, Starting, Running, Stopping, Error)
- Configurable cycle time with timing monitoring
- Watchdog timer support
- Cycle overrun detection
- Virtual methods for custom logic implementation
- Platform-independent timing (Arduino and generic)

##### DigitalIO (v1.0.0)
- Digital input handling with debouncing
- Edge detection (rising/falling)
- Digital output control
- Toggle and pulse functionality
- Header-only initial implementation

##### Timer (v1.0.0)
- Software timer for non-blocking delays
- IEC 61131-3 compliant timer functions:
  - TON (On-delay timer)
  - TOF (Off-delay timer)
  - TP (Pulse timer)
- Header-only initial implementation

#### Examples
- **simple_plc**: Basic PLC application with digital I/O and timers
- **can_communication**: CAN bus messaging between distributed nodes
- Example README with usage instructions

#### Documentation
- Complete library reference listing all planned libraries
- Comprehensive development guide with:
  - Library structure and standards
  - Coding conventions
  - Platform compatibility guidelines
  - Testing procedures
  - Best practices
- Contributing guidelines
- Quick start guide

#### Tools
- Complete library template for creating new libraries
- Template includes all standard files and structure
- Tools README explaining usage

### Platform Support
- ESP32 family
- STM32 family
- Teensy 3.x and 4.x
- Arduino Due
- Generic ARM Cortex-M

### Documentation
- README.md with comprehensive overview
- docs/quickstart.md for getting started
- docs/development.md for library development
- docs/libraries.md listing all libraries
- CONTRIBUTING.md for contribution guidelines
- Individual library README files

## Project Information

### Repository
- **URL**: https://github.com/LeoWatti99/CANTram-Framework
- **License**: GNU General Public License v3.0
- **Language**: C++ (C++11 standard)

### Versioning Strategy
- **MAJOR**: Incompatible API changes
- **MINOR**: Backwards-compatible functionality additions
- **PATCH**: Backwards-compatible bug fixes

### Release Process
1. Update version numbers in library.json files
2. Update CHANGELOG.md with changes
3. Create git tag with version
4. Build and test on all supported platforms
5. Push to GitHub
6. Publish to PlatformIO library registry (when ready)

---

[Unreleased]: https://github.com/LeoWatti99/CANTram-Framework/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/LeoWatti99/CANTram-Framework/releases/tag/v1.0.0
