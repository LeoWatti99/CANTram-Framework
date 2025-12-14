#ifndef UART_CORE_H
#define UART_CORE_H

//#include "UARTInterface.h"
#include "CANTramCore.h"
#include "HardwareResource.h"

/**
 * @file UARTCore.h
 * @brief Abstract base for UART hardware cores and provider interface.
 * @details Define the abstract UARTCore class which concrete platform implementations must inherit.
 *          Provides methods for installation, configuration, pin setup and data IO. Also defines
 *          the UARTProvider interface used to obtain UARTCore instances.
 */

/**
 * @class UARTCore
 * @brief Base class for platform-specific UART implementations.
 * @details Derive from UARTCore when implementing UART support for a specific microcontroller or platform.
 *          The class integrates with the HardwareResource lifecycle provided by CANTramCore.
 */
class UARTCore : public HardwareResource {
    public:
    /**
     * @brief Possible data bit widths for UART communication.
     * @details Enum values map to typical UART data length settings.
     */
    enum DataBits{
        UART_DATA_5_BITS = 0,
        UART_DATA_6_BITS = 1,
        UART_DATA_7_BITS = 2,
        UART_DATA_8_BITS = 3
    };
    /**
     * @brief Parity configuration options.
     * @details Select no parity, even parity or odd parity for UART frames.
     */
    enum Parity {
        UART_PARITY_NONE = 0,
        UART_PARITY_EVEN = 1,
        UART_PARITY_ODD  = 2
    };
    /**
     * @brief Stop bit configuration options.
     * @details Specifies 1, 1.5 or 2 stop bits for UART frames.
     */
    enum StopBits {
        UART_STOP_BITS_1   = 1,
        UART_STOP_BITS_1_5 = 2,
        UART_STOP_BITS_2   = 3
    };

    /**
     * @brief Install and initialize the UART driver and resources.
     * @details Prepare the underlying hardware driver and allocate required memory/buffers for UART operation.
     *          This method should be called once before attempting to use the UART for I/O.
     * @param uart_num Identifier of the UART peripheral to install (platform-specific).
     * @param tx_pin GPIO pin used for UART TX.
     * @param rx_pin GPIO pin used for UART RX.
     * @param bufferSize Size in bytes of the RX buffer to allocate.
     * @return true if installation and allocation succeeded, false on failure.
     */
    virtual bool install(int uart_num, uint8_t tx_pin, uint8_t rx_pin, uint16_t bufferSize) = 0; //Prepare the driver and allocate memory

    /**
     * @brief Reset the UART core and release/clear resources.
     * @details Default implementation logs the reset and delegates to HardwareResource::reset()
     *          to clear usage counters. Override if additional platform-specific teardown is required.
     * @return true if reset succeeded, false otherwise.
     */
    virtual bool reset() override{
        DEBUG_PRINTLN("[UARTCore] Resetting UARTCore");
        return HardwareResource::reset();
    };

    /**
     * @brief Configure UART communication parameters.
     * @details Set baudrate, data bits, parity and stop bits for the UART peripheral.
     * @param baudrate Desired baud rate (e.g., 115200).
     * @param dataBits Data bit selection from DataBits enum.
     * @param parity Parity selection from Parity enum.
     * @param stopBits Stop bit selection from StopBits enum.
     * @return true if configuration succeeded, false otherwise.
     */
    virtual bool config(uint32_t baudrate, DataBits dataBits, Parity parity, StopBits stopBits){
        DEBUG_PRINTLN("[UARTCore] Configuring UARTCore");
        _baudrate = baudrate;
        _dataBits = dataBits;
        _parity = parity;
        _stopBits = stopBits;
        return true;
    };
    virtual bool setBaudrate(uint32_t baudrate){
        _baudrate = baudrate;
        return true;
    };
    virtual bool setDataBits(DataBits dataBits){
        _dataBits = dataBits;
        return true;
    };
    virtual bool setParity(Parity parity){
        _parity = parity;
        return true;
    }; 
    virtual bool setStopBits(StopBits stopBits){
        _stopBits = stopBits;
        return true;
    };

    /**
     * @brief Send a buffer of bytes over UART.
     * @details Write the provided buffer to the UART transmit queue or driver. Default implementation logs and returns false.
     * @param buffer Character buffer containing bytes to send.
     * @param size Number of bytes to send from the buffer.
     * @return true if the data was successfully queued/sent, false otherwise.
     */
    virtual bool send(char buffer[], size_t size){DEBUG_PRINTLN("[UARTCore] Send"); return false;};

    /**
     * @brief Read available bytes from the UART into the provided buffer.
     * @details Attempt to read up to the size of the provided buffer. The concrete implementation should return
     *          the actual number of bytes read.
     * @param buffer Destination buffer to receive bytes.
     * @return size_t Number of bytes read into the buffer (0 if none available).
     */
    virtual size_t read(char buffer[]){DEBUG_PRINTLN("[UARTCore] Read"); return 0;};

    /**
     * @brief Get the number of bytes currently available for reading.
     * @details Query the UART driver's RX buffer to determine how many bytes can be read without blocking.
     * @return size_t Number of bytes available to read.
     */
    virtual size_t available()=0;

    /**
     * @brief Flush the UART transmit buffer.
     * @details Ensure that any queued transmit data is sent. Implementation should block until transmit is complete if required.
     * @return void
     */
    virtual void flush()=0;

    /**
     * @brief Get the HardwareResource type for this core.
     * @details Overrides HardwareResource::getType() to return HardwareResource::Type::UART.
     * @return HardwareResource::Type The enum value Type::UART.
     */
    HardwareResource::Type getType() override { return Type::UART; }

    virtual bool setPins(uint8_t tx_pin, uint8_t rx_pin){
        _txPin = tx_pin;
        _rxPin = rx_pin;
        pinsConfigured = true;
        return true;
    }

    protected:

    //Hardware
    int8_t _rxPin= DEFAULT_REF;
    int8_t _txPin= DEFAULT_REF;
    bool pinsConfigured = false;
    //Logic
    uint32_t _baudrate = 0;
    StopBits _stopBits = UART_STOP_BITS_1;
    Parity _parity = UART_PARITY_NONE;
    DataBits _dataBits = UART_DATA_8_BITS;
    
};

/**
 * @class UARTProvider
 * @brief Abstract interface for objects that supply a UARTCore instance.
 * @details Implement this interface for modules or systems that can provide access to a UARTCore.
 */
class UARTProvider {
    public:
        /**
         * @brief Obtain a pointer to an available UARTCore.
         * @details Returns a pointer to a UARTCore instance that can be used by clients. May return nullptr
         *          if no UARTCore is available.
         * @return UARTCore* Pointer to a UARTCore instance or nullptr if none available.
         */
        virtual UARTCore* getUartCore()=0;
};

#endif