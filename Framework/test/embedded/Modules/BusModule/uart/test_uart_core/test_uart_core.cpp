#include <Arduino.h>
#include <unity.h>
#include "ESP32_UART.h"
#include "UARTInterface.h"
#include "driver/uart.h"

ESP32_UART uart;
UARTInterface uartInterface;

uint32_t baudrate;
UARTCore::DataBits dataBits;
UARTCore::Parity parity;
UARTCore::StopBits stopBits;
uart_port_t uartPort;

int RX_PIN = 23;
int TX_PIN = 19;

//Runs before tests
void setUp(){
    uart = ESP32_UART();
    baudrate = 115200;
    dataBits = UARTCore::UART_DATA_8_BITS;
    parity = UARTCore::UART_PARITY_NONE;
    stopBits = UARTCore::UART_STOP_BITS_1;
    uartPort = UART_NUM_2;
}

//Runs after tests
void tearDown(){
    uart_driver_delete(uartPort);
}

/* Write a function for each test*/
void test_uart_install(){
    bool result = uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    TEST_ASSERT_TRUE(result);
}

void test_uart_config(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    bool result = uart.config(baudrate, dataBits, parity, stopBits);
   
    TEST_ASSERT_TRUE(result);
}

void test_uart_setPins(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uart.config(baudrate, dataBits, parity, stopBits);
    bool result = uart.setPins(TX_PIN, RX_PIN);
    TEST_ASSERT_TRUE(result);
}

void test_uart_no_data_after_start(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uart.config(baudrate, dataBits, parity, stopBits);
    uart.setPins(TX_PIN, RX_PIN);
    size_t result = uart.available();
    TEST_ASSERT_EQUAL(0, result); // Assuming no data is available initially
}



//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_uart_install);
    RUN_TEST(test_uart_config);
    RUN_TEST(test_uart_setPins);
    RUN_TEST(test_uart_no_data_after_start);
    UNITY_END();
}

void loop(){

}