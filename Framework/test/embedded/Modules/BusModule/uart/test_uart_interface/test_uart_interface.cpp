#include <Arduino.h>
#include <unity.h>
#include "ESP32_UART.h"
#include "UARTInterface.h"
#include "driver/uart.h"
#include "TestingUtility.h"

ESP32_UART uart;
UARTInterface uartInterface;

uint32_t baudrate;
UARTCore::DataBits dataBits;
UARTCore::Parity parity;
UARTCore::StopBits stopBits;
uart_port_t uartPort;
char randomBuffer[50];
String allSendData;
String allReceivedData;
size_t randomStringLength;


int RX_PIN = 23;
int TX_PIN = 19;



//Runs before tests
void setUp(){
    uart = ESP32_UART();
    uartInterface = UARTInterface();
    baudrate = 115200;
    dataBits = UARTCore::UART_DATA_8_BITS;
    parity = UARTCore::UART_PARITY_NONE;
    stopBits = UARTCore::UART_STOP_BITS_1;
    uartPort = UART_NUM_1;
    randomStringLength = random(1,49);
    generateRandomString(randomBuffer, randomStringLength); // Fill sendBuffer with random data
}





//Runs after tests
void tearDown(){
    clearBuffer(randomBuffer, sizeof(randomBuffer));
    uart.~ESP32_UART();
}

/* Write a function for each test*/
void test_interface_begin(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uartInterface.setUartCore(&uart);
    uartInterface.setBaudrate(baudrate);
    uartInterface.setDataBits(dataBits);
    uartInterface.setParity(parity);
    bool result = uartInterface.begin();
    TEST_ASSERT_TRUE(result);
}

void test_interface_send(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uartInterface.setUartCore(&uart);
    uartInterface.setBaudrate(baudrate);
    uartInterface.setDataBits(dataBits);
    uartInterface.setParity(parity);
    uartInterface.begin();
    bool result = uartInterface.send(randomBuffer, randomStringLength);
    allSendData += String(randomBuffer).substring(0, randomStringLength);
    DEBUG_PRINTLN("All sent data so far: " + allSendData);

    TEST_ASSERT_TRUE(result);

}

void test_interface_available_initial(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uartInterface.setUartCore(&uart);
    uartInterface.setBaudrate(baudrate);
    uartInterface.setDataBits(dataBits);
    uartInterface.setParity(parity);
    uartInterface.begin();
    uartInterface.available(); // Call once to check for data
    size_t result = uartInterface.available();  //Call twice to make sure, data is still marked as received
    TEST_ASSERT_EQUAL(0, result); // Assuming no data is available initially
}

void test_interface_available_after_send(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uartInterface.setUartCore(&uart);
    uartInterface.setBaudrate(baudrate);
    uartInterface.setDataBits(dataBits);
    uartInterface.setParity(parity);
    uartInterface.begin();
    DEBUG_PRINTLN("Sending: " + String(randomBuffer));
    uartInterface.send(randomBuffer, randomStringLength);
    allSendData += String(randomBuffer).substring(0, randomStringLength);
    DEBUG_PRINTLN("All sent data so far: " + allSendData);
    delay(1000); // Wait for data to be processed
    size_t result = uartInterface.available();
    TEST_ASSERT_EQUAL(randomStringLength, result); // Assuming randomStringLength bytes were sent and are now available
}

void test_interface_read_after_send(){
    uart.install(uartPort, TX_PIN, RX_PIN, 1024);
    uartInterface.setUartCore(&uart);
    uartInterface.setBaudrate(baudrate);
    uartInterface.setDataBits(dataBits);
    uartInterface.setParity(parity);
    uartInterface.begin();
    delay(3000);

    DEBUG_PRINTLN("[TEST] Sending: " + String(randomBuffer));
    uartInterface.send(randomBuffer, 50);
    allSendData += String(randomBuffer).substring(0, randomStringLength);
    DEBUG_PRINTLN("All sent data so far: " + allSendData);
    delay(3000); // Wait for data to be processed

    char buffer[50]; // +1 for null terminator
    DEBUG_PRINTLN("[TEST] Initialbuffer: " + String(buffer));
    size_t bytesRead = uartInterface.read(buffer);
    allReceivedData += String(buffer).substring(0, bytesRead);
    DEBUG_PRINTLN("All received data so far: " + allReceivedData);
    buffer[bytesRead] = '\0'; // Null-terminate the received string
    DEBUG_PRINTLN("[TEST] Received: " + String(buffer));

    TEST_ASSERT_EQUAL_STRING(randomBuffer, buffer);
}

//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_interface_begin);
    RUN_TEST(test_interface_send);
    RUN_TEST(test_interface_available_initial);
    RUN_TEST(test_interface_available_after_send);
    RUN_TEST(test_interface_read_after_send);
    UNITY_END();
    DEBUG_PRINTLN("All sent data:     " + allSendData);
    DEBUG_PRINTLN("All received data: " + allReceivedData);
}

void loop(){

}