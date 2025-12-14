#include "ESP32_UART.h"
#include <driver/uart.h>
#include "Debug.h"
#include "UARTInterface.h"

/**
 * @brief Reset the ESP32 UART instance.
 * @details Releases driver resources, flushes buffers and returns the internal state to NONE.
 *          Safe to call if driver is not installed; performs no action in that case.
 * @return true on successful reset.
 */
bool ESP32_UART::reset(){
    UARTCore::reset();
    if(_state == NONE) {
        DEBUG_PRINTLN("[ESP32_UART] UART already in NONE state, nothing to reset.");
        return true; // Nothing to reset
    }
    if(!uart_is_driver_installed(_uartPort)) {
        DEBUG_PRINTLN("[ESP32_UART] UART driver not installed, nothing to reset.");
        _state = NONE;
        return true; // Nothing to reset
    }
    INFO_PRINTLN("[ESP32_UART] Resetting UART " + String(_uartPort) + "...");
    ESP_ERROR_CHECK(uart_flush(_uartPort));
    ESP_ERROR_CHECK(uart_flush_input(_uartPort));
    ESP_ERROR_CHECK(uart_driver_delete(_uartPort));
    _state = NONE;
    _rxPin = DEFAULT_REF;
    _txPin = DEFAULT_REF;
    _uartPort = DEFAULT_REF;
    INFO_PRINTLN("[ESP32_UART] UART Reset complete");
    return true;
}

/**
 * @brief Install the UART driver and prepare the instance.
 * @details Installs the UART driver for the given uart_num if not already installed,
 *          stores pin and port configuration and marks the instance state as INSTALLED.
 * @param uart_num UART peripheral number to install.
 * @param tx_pin GPIO pin used for TX.
 * @param rx_pin GPIO pin used for RX.
 * @param bufferSize Size in bytes for RX/TX buffers.
 * @return true if installation succeeded, false otherwise.
 */
bool ESP32_UART::install(int uart_num, uint8_t tx_pin, uint8_t rx_pin, uint16_t bufferSize) {
    
    

    //Install UART driver
    if(!uart_is_driver_installed(uart_num)) {
        ESP_ERROR_CHECK(uart_driver_install(uart_num, bufferSize, bufferSize, 10, &uart_queue, 0));
        if(!uart_is_driver_installed(uart_num)) {
            ERROR_PRINTLN("[ESP32_UART] Failed to install UART driver");
            return false;
        }
    }
    
    _rxPin = rx_pin;
    _txPin = tx_pin;
    _uartPort = uart_num;
    _state = INSTALLED;
    return true;
}
    
/**
 * @brief Configure UART communication parameters.
 * @details Applies baudrate, data bits, parity and stop bits to the UART peripheral.
 *          Requires the instance to be in INSTALLED state prior to calling.
 * @param baudrate Desired baud rate (e.g., 115200).
 * @param dataBits Data bits selection from UARTCore::DataBits.
 * @param parity Parity selection from UARTCore::Parity.
 * @param stopBits Stop bits selection from UARTCore::StopBits.
 * @return true if configuration succeeded, false otherwise.
 */
bool ESP32_UART::applyConfig() {
    if(_state != INSTALLED) {
        ERROR_PRINTLN("[ESP32_UART] UART must be installed before configuration");
        return false;
    }
    
    uart_config_t uart_config = {
        .baud_rate = _baudrate,
        .data_bits = toEsp32DataBits(_dataBits),
        .parity    = toEsp32Parity(_parity),
        .stop_bits = toEsp32StopBits(_stopBits),
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
    };
    ESP_ERROR_CHECK(uart_param_config(_uartPort, &uart_config));
    if(false) {
        DEBUG_PRINTLN("[ESP32_UART] Failed to configure UART parameters");
        return false;
    }
    _state = CONFIGURED;
    return true;
}

/**
 * @brief Set TX and RX pins and start the UART peripheral.
 * @details Configures the UART peripheral to use the provided pins and transitions the state to STARTED.
 *          Requires the instance to be in CONFIGURED state.
 * @param tx_pin GPIO pin number to use for TX.
 * @param rx_pin GPIO pin number to use for RX.
 * @return true if pins were set and UART started successfully, false otherwise.
 */
bool ESP32_UART::applyPins(int tx_pin, int rx_pin) {
    UARTCore::_txPin = tx_pin;
    UARTCore::_rxPin = rx_pin;
    if(_state != CONFIGURED) {
        ERROR_PRINTLN("[ESP32_UART] UART must be configured before setting pins");
        return false;
    }
    ESP_ERROR_CHECK(uart_set_pin(_uartPort, tx_pin, rx_pin, -1, -1));
    if(false) {
        ERROR_PRINTLN("[ESP32_UART] Failed to set UART pins");
        return false;
    }
    
    _state = STARTED;
    INFO_PRINTLN("[ESP32_UART] UART started successfully on TX pin " + String(tx_pin) + " and RX pin " + String(rx_pin));
    return true;
}

/**
 * @brief Apply previously stored TX/RX pin mapping.
 * @details Overload that calls setPins using stored _txPin/_rxPin values.
 * @return true if pins were applied successfully, false otherwise.
 */
bool ESP32_UART::applyPins() {
    return applyPins(_txPin, _rxPin);
}


/**
 * @brief Send a buffer of bytes over UART.
 * @details Writes the provided buffer to the UART transmit queue. Requires the UART to be STARTED.
 * @param buffer Character buffer containing bytes to send.
 * @param size Number of bytes to send from the buffer.
 * @return true if data was queued/sent successfully, false otherwise.
 */
bool ESP32_UART::send(const char buffer[], size_t size) {
    UARTCore::send(buffer, size);
    if(_state != STARTED) {
        ERROR_PRINTLN("[ESP32_UART] UART must be started before sending data");
        return false;
    }
    int len = uart_write_bytes(_uartPort, buffer, size);
    if(len < 0) {
        ERROR_PRINTLN("[ESP32_UART] Failed to send data over UART");
        return false;
    }
    return true;
}

/**
 * @brief Read available bytes from UART into buffer.
 * @details Attempts to read up to the number of bytes currently available. Requires UART to be STARTED.
 * @param buffer Destination buffer to receive bytes.
 * @return size_t Number of bytes read into the buffer (0 if none).
 */
size_t ESP32_UART::read(char buffer[]){
    UARTCore::read(buffer);
    if(_state != STARTED) {
        ERROR_PRINTLN("[ESP32_UART] UART must be started before reading data");
        return 0;
    }

    size_t num = available();
    if(num == 0) return 0;

    num = uart_read_bytes(_uartPort,buffer,num,100);
    return num;
}

/**
 * @brief Get number of bytes available to read.
 * @details Queries the UART driver's buffered data length and returns the number of bytes ready for reading.
 * @return size_t Number of bytes available.
 */
size_t ESP32_UART::available(){
    size_t receivedBytes;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(_uartPort,&receivedBytes));
    DEBUG_PRINTLN("[ESP32_UART] Bytes available: " + String(receivedBytes));
    return receivedBytes;
}

/**
 * @brief Flush UART RX and TX buffers.
 * @details Ensures any queued transmit data is flushed and input buffers are cleared.
 * @return void
 */
void ESP32_UART::flush(){
    ESP_ERROR_CHECK(uart_flush(_uartPort));
    ESP_ERROR_CHECK(uart_flush_input(_uartPort));
    DEBUG_PRINTLN("[ESP32_UART] UART RX & TX buffer flushed");
}

