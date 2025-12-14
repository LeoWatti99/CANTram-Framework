# Development Guide

This guide explains how to develop libraries for the CANTram Framework.

## Library Structure

Each library in the CANTram Framework follows a standardized structure:

```
lib/YourLibrary/
├── src/
│   ├── YourLibrary.h      # Header file with class declarations
│   └── YourLibrary.cpp    # Implementation file
├── examples/
│   └── basic_usage.cpp    # Example usage
├── README.md              # Library documentation
└── library.json           # PlatformIO library manifest
```

### Optional Files

- `CMakeLists.txt` - For CMake build support
- `keywords.txt` - Arduino IDE syntax highlighting
- `library.properties` - Arduino library manager

## Creating a New Library

### Step 1: Create Directory Structure

```bash
mkdir -p lib/YourLibrary/src
mkdir -p lib/YourLibrary/examples
```

### Step 2: Create library.json

Create a `library.json` file with the following template:

```json
{
  "name": "YourLibrary",
  "version": "1.0.0",
  "description": "Brief description of your library",
  "keywords": ["keyword1", "keyword2"],
  "authors": [
    {
      "name": "Your Name",
      "maintainer": true
    }
  ],
  "repository": {
    "type": "git",
    "url": "https://github.com/LeoWatti99/CANTram-Framework.git"
  },
  "license": "GPL-3.0",
  "frameworks": ["arduino", "mbed", "espidf"],
  "platforms": ["espressif32", "ststm32", "teensy", "atmelsam"],
  "export": {
    "include": ["src"]
  }
}
```

### Step 3: Create Header File

Create `src/YourLibrary.h`:

```cpp
/**
 * @file YourLibrary.h
 * @brief Brief description
 * @author Your Name
 * @version 1.0.0
 * @date 2024
 */

#ifndef YOURLIBRARY_H
#define YOURLIBRARY_H

#include <stdint.h>
#include <stdbool.h>

class YourLibrary {
public:
    YourLibrary();
    ~YourLibrary();
    
    bool begin();
    void end();
    
    // Your public methods here

private:
    // Your private members here
};

#endif // YOURLIBRARY_H
```

### Step 4: Create Implementation File

Create `src/YourLibrary.cpp`:

```cpp
/**
 * @file YourLibrary.cpp
 * @brief Implementation of YourLibrary
 * @author Your Name
 */

#include "YourLibrary.h"

YourLibrary::YourLibrary() {
    // Constructor implementation
}

YourLibrary::~YourLibrary() {
    // Destructor implementation
}

bool YourLibrary::begin() {
    // Initialization code
    return true;
}

void YourLibrary::end() {
    // Cleanup code
}
```

### Step 5: Create README

Create a comprehensive `README.md` for your library:

```markdown
# YourLibrary

Brief description of your library.

## Features

- Feature 1
- Feature 2

## Installation

Include in your project:
\`\`\`cpp
#include <YourLibrary.h>
\`\`\`

## Usage

\`\`\`cpp
YourLibrary lib;

void setup() {
    lib.begin();
}

void loop() {
    // Use library
}
\`\`\`

## API Reference

Document your public methods here.

## License

GPL-3.0
```

### Step 6: Create Example

Create `examples/basic_usage.cpp`:

```cpp
#include <Arduino.h>
#include <YourLibrary.h>

YourLibrary lib;

void setup() {
    Serial.begin(115200);
    
    if (lib.begin()) {
        Serial.println("Library initialized");
    }
}

void loop() {
    // Example usage
    delay(1000);
}
```

## Coding Standards

### Naming Conventions

- **Classes**: PascalCase (e.g., `CANCore`, `PLCCore`)
- **Methods**: camelCase (e.g., `sendMessage`, `getState`)
- **Private members**: Underscore prefix (e.g., `_state`, `_baudrate`)
- **Constants**: UPPER_CASE (e.g., `CAN_500KBPS`)
- **Enums**: PascalCase for type, UPPER_CASE for values

### Code Style

```cpp
// Use K&R style braces
void myFunction() {
    if (condition) {
        // code
    } else {
        // code
    }
}

// Use descriptive variable names
uint32_t messageCount = 0;  // Good
uint32_t mc = 0;             // Bad

// Comment complex logic
// This algorithm implements XYZ according to spec ABC
complexAlgorithm();

// Use const for immutable parameters
bool processMessage(const CANMessage* msg);
```

### Documentation

Use Doxygen-style comments:

```cpp
/**
 * @brief Brief description
 * @param paramName Description of parameter
 * @return Description of return value
 * @note Additional notes
 * @warning Important warnings
 */
bool myFunction(int paramName);
```

## Platform Compatibility

### Platform Detection

Use preprocessor directives for platform-specific code:

```cpp
#if defined(ESP32)
    // ESP32 specific code
#elif defined(STM32)
    // STM32 specific code
#elif defined(TEENSY)
    // Teensy specific code
#elif defined(ARDUINO_SAM_DUE)
    // Arduino Due specific code
#else
    // Generic/fallback code
#endif
```

### Arduino vs Non-Arduino

```cpp
#if defined(ARDUINO)
    #include <Arduino.h>
    #define GET_MILLIS() millis()
#else
    #include <chrono>
    #define GET_MILLIS() (chrono::milliseconds)
#endif
```

## Testing

### Unit Tests

Create unit tests for your library:

```cpp
// tests/test_yourlibrary.cpp
#include <unity.h>
#include <YourLibrary.h>

void test_initialization() {
    YourLibrary lib;
    TEST_ASSERT_TRUE(lib.begin());
}

void test_functionality() {
    // Test your library functions
    TEST_ASSERT_EQUAL(expected, actual);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_functionality);
    UNITY_END();
}

void loop() {
    // Empty
}
```

### Manual Testing

Create example sketches that demonstrate and test functionality.

## Best Practices

### Memory Management

- Avoid dynamic memory allocation when possible
- Use stack allocation for small objects
- If using heap, ensure proper cleanup in destructor
- Be mindful of memory constraints on microcontrollers

### Performance

- Minimize CPU usage in frequently-called functions
- Use appropriate data types (uint8_t vs uint32_t)
- Avoid floating-point math when possible
- Profile code on target hardware

### Error Handling

```cpp
class MyLibrary {
public:
    bool operation() {
        if (!validate()) {
            _lastError = ERROR_CODE;
            return false;
        }
        // Perform operation
        return true;
    }
    
    uint8_t getLastError() const {
        return _lastError;
    }

private:
    uint8_t _lastError;
};
```

### Resource Management

- Always provide `begin()` and `end()` methods
- Initialize in `begin()`, not in constructor
- Clean up resources in `end()` and destructor
- Use RAII when appropriate

## Building

### PlatformIO

```bash
# Build for specific environment
pio run -e esp32dev

# Build all environments
pio run

# Upload to device
pio run -t upload

# Run tests
pio test
```

### CMake

```bash
mkdir build && cd build
cmake ..
make
```

## Documentation

Generate documentation with Doxygen:

```bash
doxygen docs/Doxyfile
```

## Submitting Your Library

1. Test thoroughly on multiple platforms
2. Write comprehensive documentation
3. Create meaningful examples
4. Follow coding standards
5. Submit pull request

## Support

For questions or issues:
- Open an issue on GitHub
- Check existing documentation
- Review example libraries

## Resources

- [PlatformIO Library Documentation](https://docs.platformio.org/en/latest/librarymanager/)
- [Arduino Library Specification](https://arduino.github.io/arduino-cli/latest/library-specification/)
- [IEC 61131-3 Standard](https://en.wikipedia.org/wiki/IEC_61131-3)
