/**
 * @file main.cpp
 * @brief Simple PLC example using CANTram Framework
 * 
 * This example demonstrates basic PLC functionality with digital I/O
 * and timer operations.
 * 
 * Hardware Setup:
 * - Digital Input on Pin 2 (Start Button)
 * - Digital Input on Pin 3 (Stop Button)
 * - Digital Output on Pin 13 (Motor)
 * - Digital Output on Pin 12 (Status LED)
 */

#include <Arduino.h>
#include <PLCCore.h>
#include <DigitalIO.h>
#include <Timer.h>

// Pin definitions
#define PIN_START_BTN   2
#define PIN_STOP_BTN    3
#define PIN_MOTOR       13
#define PIN_STATUS_LED  12

// Create instances
class SimplePLC : public PLCCore {
public:
    SimplePLC() : PLCCore(10) {}  // 10ms cycle time
    
    void init() {
        startBtn.begin();
        stopBtn.begin();
        motorOutput.begin();
        statusLED.begin();
        motorTimer.setPreset(5000);  // 5 second delay
    }

protected:
    void scanInputs() override {
        // Read input buttons
        startBtn.update();
        stopBtn.update();
    }
    
    void executeLogic() override {
        // Start button pressed (rising edge)
        if (startBtn.risingEdge()) {
            motorRunning = true;
            Serial.println("Motor START command");
        }
        
        // Stop button pressed (rising edge)
        if (stopBtn.risingEdge()) {
            motorRunning = false;
            Serial.println("Motor STOP command");
        }
        
        // Motor timer logic (5 second delay before motor starts)
        motorTimer.execute(motorRunning);
        
        // Status LED blinks when motor is running
        if (motorTimer.Q()) {
            // Blink status LED
            if ((getCycleCount() % 50) == 0) {  // Toggle every 500ms
                statusLED.toggle();
            }
        } else {
            statusLED.low();
        }
    }
    
    void updateOutputs() override {
        // Update motor output
        motorOutput.write(motorTimer.Q());
    }
    
    void processCommunication() override {
        // Print status every second
        if ((getCycleCount() % 100) == 0) {
            Serial.print("PLC Running - Cycle Time: ");
            Serial.print(getCycleTime());
            Serial.print(" us, Motor: ");
            Serial.println(motorTimer.Q() ? "ON" : "OFF");
        }
    }

private:
    DigitalInput startBtn{PIN_START_BTN, true, 50};
    DigitalInput stopBtn{PIN_STOP_BTN, true, 50};
    DigitalOutput motorOutput{PIN_MOTOR};
    DigitalOutput statusLED{PIN_STATUS_LED};
    TON motorTimer{5000};
    bool motorRunning = false;
};

SimplePLC plc;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=================================");
    Serial.println("CANTram Framework - Simple PLC");
    Serial.println("=================================");
    
    // Initialize PLC
    plc.init();
    
    if (plc.initialize()) {
        Serial.println("PLC initialized successfully");
        
        if (plc.start()) {
            Serial.println("PLC started");
        } else {
            Serial.println("Failed to start PLC");
        }
    } else {
        Serial.println("Failed to initialize PLC");
    }
}

void loop() {
    // Execute PLC cycle
    plc.executeCycle();
    
    // Check for cycle overrun
    if (plc.isCycleOverrun()) {
        Serial.println("WARNING: PLC cycle overrun detected!");
    }
}
