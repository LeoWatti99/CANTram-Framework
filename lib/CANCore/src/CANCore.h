/**
 * @file CANCore.h
 * @brief Core CAN bus communication library for CANTram Framework
 * @author Leo Watti
 * @version 1.0.0
 * @date 2024
 * 
 * This library provides fundamental CAN bus communication functionality
 * for distributed PLC systems based on microcontrollers.
 */

#ifndef CANCORE_H
#define CANCORE_H

#include <stdint.h>
#include <stdbool.h>

// CAN bus speed definitions
#define CAN_5KBPS      5000
#define CAN_10KBPS     10000
#define CAN_20KBPS     20000
#define CAN_50KBPS     50000
#define CAN_100KBPS    100000
#define CAN_125KBPS    125000
#define CAN_250KBPS    250000
#define CAN_500KBPS    500000
#define CAN_1000KBPS   1000000

// CAN frame types
#define CAN_STANDARD_FRAME  0
#define CAN_EXTENDED_FRAME  1

// CAN frame structure
typedef struct {
    uint32_t id;           // CAN identifier (11 or 29 bits)
    uint8_t  data[8];      // Data bytes (0-8 bytes)
    uint8_t  len;          // Data length (0-8)
    bool     extended;     // Extended frame flag
    bool     rtr;          // Remote transmission request flag
} CANMessage;

/**
 * @class CANCore
 * @brief Core CAN bus communication class
 * 
 * Provides methods for initializing and communicating over CAN bus.
 * Supports both standard (11-bit) and extended (29-bit) CAN identifiers.
 */
class CANCore {
public:
    /**
     * @brief Constructor
     * @param baudrate CAN bus speed in bps (default: 500kbps)
     */
    CANCore(uint32_t baudrate = CAN_500KBPS);
    
    /**
     * @brief Destructor
     */
    ~CANCore();
    
    /**
     * @brief Initialize CAN bus
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * @brief Stop CAN bus communication
     */
    void end();
    
    /**
     * @brief Send a CAN message
     * @param msg Pointer to CANMessage structure
     * @return true if message sent successfully, false otherwise
     */
    bool sendMessage(const CANMessage* msg);
    
    /**
     * @brief Receive a CAN message
     * @param msg Pointer to CANMessage structure to store received message
     * @return true if message received, false if no message available
     */
    bool receiveMessage(CANMessage* msg);
    
    /**
     * @brief Check if messages are available
     * @return Number of messages available in receive buffer
     */
    uint8_t available();
    
    /**
     * @brief Process pending CAN messages
     * Should be called regularly in main loop
     */
    void processMessages();
    
    /**
     * @brief Set receive filter
     * @param id CAN ID to filter
     * @param mask Filter mask
     * @param extended Use extended frame format
     */
    void setFilter(uint32_t id, uint32_t mask, bool extended = false);
    
    /**
     * @brief Clear all filters
     */
    void clearFilters();
    
    /**
     * @brief Get last error code
     * @return Error code (0 = no error)
     */
    uint8_t getLastError();
    
    /**
     * @brief Get bus statistics
     * @param txCount Pointer to store transmitted message count
     * @param rxCount Pointer to store received message count
     * @param errorCount Pointer to store error count
     */
    void getStatistics(uint32_t* txCount, uint32_t* rxCount, uint32_t* errorCount);

private:
    uint32_t _baudrate;
    uint32_t _txCount;
    uint32_t _rxCount;
    uint32_t _errorCount;
    uint8_t  _lastError;
    bool     _initialized;
    
    // Platform-specific initialization
    bool platformInit();
    void platformEnd();
};

#endif // CANCORE_H
