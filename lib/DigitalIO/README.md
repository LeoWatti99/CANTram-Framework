# DigitalIO Library

Digital input/output handling library for the CANTram Framework.

## Description

DigitalIO provides robust digital I/O handling with features like input debouncing, edge detection, and output control. It simplifies working with digital pins on microcontrollers.

## Features

### DigitalInput
- Configurable debouncing
- Edge detection (rising/falling)
- State change detection
- Pull-up/pull-down support
- Raw and filtered reading

### DigitalOutput
- Simple high/low control
- Toggle functionality
- Pulse generation
- State reading

## Installation

This library is part of the CANTram Framework. Include it in your project:

```cpp
#include <DigitalIO.h>
```

## Usage

### Digital Input Example

```cpp
#include <Arduino.h>
#include <DigitalIO.h>

// Create input with pull-up and 50ms debounce
DigitalInput button(2, true, 50);

void setup() {
    Serial.begin(115200);
    button.begin();
}

void loop() {
    button.update();  // Must be called regularly
    
    if (button.risingEdge()) {
        Serial.println("Button pressed!");
    }
    
    if (button.fallingEdge()) {
        Serial.println("Button released!");
    }
    
    if (button.isHigh()) {
        // Button is currently pressed
    }
}
```

### Digital Output Example

```cpp
#include <Arduino.h>
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

## API Reference

### DigitalInput

**Constructor:**
- `DigitalInput(uint8_t pin, bool pullup = true, uint16_t debounceMs = 50)`

**Methods:**
- `void begin()` - Initialize the input pin
- `void update()` - Update state (must be called regularly)
- `bool read()` - Get debounced state
- `bool readRaw()` - Get raw state (no debouncing)
- `bool isHigh()` - Check if input is high
- `bool isLow()` - Check if input is low
- `bool changed()` - Check if state changed since last update
- `bool risingEdge()` - Check for rising edge
- `bool fallingEdge()` - Check for falling edge

### DigitalOutput

**Constructor:**
- `DigitalOutput(uint8_t pin, bool initialState = false)`

**Methods:**
- `void begin()` - Initialize the output pin
- `void write(bool state)` - Set output state
- `void high()` - Set output high
- `void low()` - Set output low
- `void toggle()` - Toggle output state
- `bool read()` - Read current output state
- `void pulse(uint32_t durationMs)` - Generate pulse (requires periodic updates)

## License

GPL-3.0

## Author

Leo Watti
