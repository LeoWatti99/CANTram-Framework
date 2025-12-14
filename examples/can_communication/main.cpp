/**
 * @file main.cpp
 * @brief CAN bus communication example using CANTram Framework
 * 
 * This example demonstrates CAN bus communication between nodes in a
 * distributed PLC system.
 * 
 * Hardware Setup:
 * - CAN transceiver connected to CAN TX/RX pins
 * - LED on Pin 13 for status indication
 */

#include <Arduino.h>
#include <CANCore.h>
#include <PLCCore.h>

// CAN message IDs
#define CAN_ID_HEARTBEAT    0x100
#define CAN_ID_STATUS       0x200
#define CAN_ID_COMMAND      0x300
#define CAN_ID_DATA         0x400

// Node configuration
#define NODE_ID             1
#define STATUS_LED_PIN      13

// Create instances
CANCore canBus(CAN_500KBPS);
PLCCore plc(10);  // 10ms cycle time

// Global variables
uint32_t heartbeatCounter = 0;
uint32_t messagesReceived = 0;
uint32_t messagesSent = 0;
bool ledState = false;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("===========================================");
    Serial.println("CANTram Framework - CAN Communication Demo");
    Serial.println("===========================================");
    Serial.print("Node ID: ");
    Serial.println(NODE_ID);
    
    // Initialize LED
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);
    
    // Initialize CAN bus
    if (canBus.begin()) {
        Serial.println("CAN bus initialized at 500kbps");
        
        // Set filter to accept specific messages
        canBus.setFilter(0x100, 0xFF00, false);
        
    } else {
        Serial.println("ERROR: Failed to initialize CAN bus");
        Serial.print("Error code: ");
        Serial.println(canBus.getLastError());
    }
    
    // Initialize PLC
    if (plc.initialize() && plc.start()) {
        Serial.println("PLC started");
    }
    
    Serial.println("System ready");
}

void loop() {
    // Process CAN messages
    canBus.processMessages();
    
    // Receive messages
    CANMessage rxMsg;
    while (canBus.receiveMessage(&rxMsg)) {
        messagesReceived++;
        
        Serial.print("RX: ID=0x");
        Serial.print(rxMsg.id, HEX);
        Serial.print(" Len=");
        Serial.print(rxMsg.len);
        Serial.print(" Data=");
        
        for (int i = 0; i < rxMsg.len; i++) {
            Serial.print(rxMsg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        
        // Process based on message ID
        switch (rxMsg.id) {
            case CAN_ID_HEARTBEAT:
                // Heartbeat from another node
                Serial.println("  -> Heartbeat received");
                break;
                
            case CAN_ID_COMMAND:
                // Command message
                Serial.println("  -> Command received");
                if (rxMsg.len > 0 && rxMsg.data[0] == 0x01) {
                    ledState = !ledState;
                    digitalWrite(STATUS_LED_PIN, ledState);
                }
                break;
                
            case CAN_ID_STATUS:
                // Status message
                Serial.println("  -> Status update received");
                break;
                
            default:
                Serial.println("  -> Unknown message type");
                break;
        }
    }
    
    // Send heartbeat every second
    static uint32_t lastHeartbeat = 0;
    if (millis() - lastHeartbeat >= 1000) {
        lastHeartbeat = millis();
        
        CANMessage txMsg;
        txMsg.id = CAN_ID_HEARTBEAT;
        txMsg.len = 8;
        txMsg.extended = false;
        txMsg.rtr = false;
        
        // Pack heartbeat data
        txMsg.data[0] = NODE_ID;
        txMsg.data[1] = (heartbeatCounter >> 24) & 0xFF;
        txMsg.data[2] = (heartbeatCounter >> 16) & 0xFF;
        txMsg.data[3] = (heartbeatCounter >> 8) & 0xFF;
        txMsg.data[4] = heartbeatCounter & 0xFF;
        txMsg.data[5] = ledState ? 0x01 : 0x00;
        txMsg.data[6] = 0x00;
        txMsg.data[7] = 0x00;
        
        if (canBus.sendMessage(&txMsg)) {
            messagesSent++;
            Serial.print("TX: Heartbeat #");
            Serial.println(heartbeatCounter);
        } else {
            Serial.println("ERROR: Failed to send heartbeat");
        }
        
        heartbeatCounter++;
    }
    
    // Send status every 5 seconds
    static uint32_t lastStatus = 0;
    if (millis() - lastStatus >= 5000) {
        lastStatus = millis();
        
        CANMessage txMsg;
        txMsg.id = CAN_ID_STATUS;
        txMsg.len = 8;
        txMsg.extended = false;
        txMsg.rtr = false;
        
        // Pack status data
        txMsg.data[0] = NODE_ID;
        txMsg.data[1] = (messagesSent >> 8) & 0xFF;
        txMsg.data[2] = messagesSent & 0xFF;
        txMsg.data[3] = (messagesReceived >> 8) & 0xFF;
        txMsg.data[4] = messagesReceived & 0xFF;
        txMsg.data[5] = plc.getState();
        txMsg.data[6] = 0x00;
        txMsg.data[7] = 0x00;
        
        if (canBus.sendMessage(&txMsg)) {
            Serial.println("TX: Status update");
        }
    }
    
    // Print statistics every 10 seconds
    static uint32_t lastStats = 0;
    if (millis() - lastStats >= 10000) {
        lastStats = millis();
        
        uint32_t txCount, rxCount, errorCount;
        canBus.getStatistics(&txCount, &rxCount, &errorCount);
        
        Serial.println("\n=== CAN Bus Statistics ===");
        Serial.print("TX Messages: ");
        Serial.println(txCount);
        Serial.print("RX Messages: ");
        Serial.println(rxCount);
        Serial.print("Errors: ");
        Serial.println(errorCount);
        Serial.print("PLC Cycle Time: ");
        Serial.print(plc.getCycleTime());
        Serial.println(" us");
        Serial.println("==========================\n");
    }
    
    delay(10);
}
