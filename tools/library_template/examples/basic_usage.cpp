/**
 * @file basic_usage.cpp
 * @brief Basic usage example for LibraryName
 * 
 * This example demonstrates the basic functionality of LibraryName.
 */

#include <Arduino.h>
#include <LibraryName.h>

// Create instance
LibraryName myLib(1000);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("LibraryName Basic Usage Example");
    Serial.println("================================");
    
    // Initialize the library
    if (myLib.begin()) {
        Serial.println("Library initialized successfully");
    } else {
        Serial.println("Failed to initialize library");
        while (1) {
            delay(1000);
        }
    }
}

void loop() {
    // Use library methods
    if (myLib.exampleMethod(123)) {
        Serial.println("Method executed successfully");
    }
    
    // Get status
    uint8_t status = myLib.getStatus();
    Serial.print("Status: ");
    Serial.println(status);
    
    delay(1000);
}
