#ifndef ESP32_UART_H
#define ESP32_UART_H

#include <driver/uart.h>
#include "UARTInterface.h"
#include "UARTCore.h"

/*
    @brief Class to manage UART Objects on ESP32
*/
class ESP32_UART : public UARTCore {
    public:

        enum uartState {
        NONE,
        INSTALLED,
        CONFIGURED,
        STARTED
        };
        ESP32_UART() = default;
        virtual bool reset() override;
        
        bool install(
            int uart_num,
            uint8_t tx_pin, 
            uint8_t rx_pin, 
            uint16_t bufferSize
        ) override; //Prepare the driver and allocate memory

        bool applyConfig(); //Start the driver
         bool applyPins(int tx_pin, int rx_pin); //Set the TX and RX pins
        bool applyPins(); 
        void setUARTPort(uart_port_t uartPort) { _uartPort = uartPort; }

        bool send(char buffer[], size_t size) override;
        size_t read(char buffer[]) override;
        size_t available() override;
        void flush() override;
    private:
        uartState _state = NONE;
        QueueHandle_t uart_queue = nullptr;
        uart_port_t _uartPort=DEFAULT_REF;

        uart_word_length_t toEsp32DataBits(UARTCore::DataBits bits) {
            switch (bits) {
            case UARTCore::DataBits::UART_DATA_5_BITS: return uart_word_length_t(UART_DATA_5_BITS);
            case UARTCore::DataBits::UART_DATA_6_BITS: return uart_word_length_t(UART_DATA_6_BITS);
            case UARTCore::DataBits::UART_DATA_7_BITS: return uart_word_length_t(UART_DATA_7_BITS);
            case UARTCore::DataBits::UART_DATA_8_BITS: return uart_word_length_t(UART_DATA_8_BITS);
            default: return uart_word_length_t(UART_DATA_8_BITS); // fallback or handle error
            }
        }

        uart_parity_t toEsp32Parity(UARTCore::Parity parity) {
            switch (parity) {
            case UARTCore::Parity::UART_PARITY_NONE: return uart_parity_t(UART_PARITY_DISABLE);
            case UARTCore::Parity::UART_PARITY_EVEN: return uart_parity_t(UART_PARITY_EVEN);
            case UARTCore::Parity::UART_PARITY_ODD:  return uart_parity_t(UART_PARITY_ODD);
            default: return uart_parity_t(UART_PARITY_DISABLE); // fallback or handle error
            }
        }

        uart_stop_bits_t toEsp32StopBits(UARTCore::StopBits stopBits) {
            switch (stopBits) {
            case UARTCore::StopBits::UART_STOP_BITS_1:   return uart_stop_bits_t(UART_STOP_BITS_1);
            case UARTCore::StopBits::UART_STOP_BITS_1_5: return uart_stop_bits_t(UART_STOP_BITS_1_5);
            case UARTCore::StopBits::UART_STOP_BITS_2:   return uart_stop_bits_t(UART_STOP_BITS_2);
            default: return uart_stop_bits_t(UART_STOP_BITS_1); // fallback or handle error
            }
        }


};

#endif