#include <Arduino.h>
#include <unity.h>
#include <TestingUtility.h>
#include "BusModule.h"
#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "Debug.h"
#include "UARTInterface.h"
#include "I2CInterface.h"
#include "Interface.h"
#include "../test/CANTramTestSetup.h"

MainModuleV1_0 mainmodule;
BusModuleV1_0 busmodule;

size_t randomStringLength;
char randomBuffer[50];

static int loopCalls=0;



//Runs before tests
void setUp(){
    CANTramCore::attachModule(&mainmodule);
    setupSPITestEnvironment();
    DEBUG_PRINTLN("setUp() complete");
}

//Runs after tests
void tearDown(){
    CANTramCore::reset();
    DEBUG_PRINTLN("tearDown() complete");
    clearBuffer(randomBuffer,randomStringLength);
}
void test_busmodule_check_for_mainmodule(){
    DEBUG_PRINTLN("Running test_busmodule_check_for_mainmodule");
    TEST_ASSERT_NOT_NULL(CANTramCore::useHardwareResource(HardwareResource::Type::UART)); // Check if UART resource is attached
    
}
void test_busmodule_attachModule(){
    DEBUG_PRINTLN("Running test_busmodule_attachModule");
    CANTramCore::attachModule(&busmodule);
    
}

void test_busmodule_initialize(){
    CANTramCore::attachModule(&busmodule);

    bool result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);    
}

void test_busModule_get_interfaces(){
    CANTramCore::attachModule(&busmodule);
    Interface** interfaces = busmodule.getInterfaces();
    TEST_ASSERT_NOT_NULL(interfaces);

    
    //Check Uart interface
    UARTInterface* uartInterface = busmodule.getInterfaces()[0]->as<UARTInterface*>();
    HardwareResource* core = uartInterface->getUartCore();
    TEST_ASSERT_EQUAL_UINT8(0,core->getRemainingUsages()); // Check if UART resource is attached
    TEST_ASSERT_TRUE(uartInterface->isValid());

    //Check i2c interface
    I2CInterface* i2cInterface = busmodule.getInterfaces()[1]->as<I2CInterface*>();
    I2CCore* i2cCore = i2cInterface->getI2CCore();
    TEST_ASSERT_TRUE(i2cInterface->isValid());
}

void loop_callback(uint8_t* response){
    loopCalls++;
    DEBUG_PRINTLN("Inside loop callback of BusModuleV1_0");
}

void measurment_busmodule_loopDuration(){
    CANTramCore::attachModule(&busmodule);
    CANTramCore::initialize();

    busmodule.setLoopFunction(loop_callback);

    const uint8_t MEASUREMENTS = 100;
    MeasurementArray<MEASUREMENTS> durations;

    for(uint8_t i=0;i<MEASUREMENTS;i++){
        uint32_t startTime = esp_timer_get_time();
        CANTramCore::loop();
        uint32_t endTime = esp_timer_get_time();
        durations.addSample(endTime - startTime);
    }

    MEASUREMENT_PRINTLN("Measured CANTramCore::loop() durations over " + String(MEASUREMENTS) + " cycles:");
    MEASUREMENT_PRINTLN("  Average duration: " + String(durations.getAverage()) + " us");
    MEASUREMENT_PRINTLN("  Max duration: " + String(durations.getMax()) + " us");
    MEASUREMENT_PRINTLN("  Min duration: " + String(durations.getMin()) + " us");
    TEST_ASSERT_LESS_OR_EQUAL(2000, durations.getMax()); // Ensure max duration is less than 2ms
}

void test_busmodule_send_data_uart(){
    CANTramCore::attachModule(&busmodule);
    UARTInterface* uartInterface = busmodule.getInterfaces()[0]->as<UARTInterface*>();
    DEBUG_PRINTLN("Setting UART Paramters...");
    uartInterface->setBaudrate(115200);
    uartInterface->setDataBits(UARTCore::UART_DATA_8_BITS);
    uartInterface->setParity(UARTCore::UART_PARITY_NONE);
    uartInterface->setStopBits(UARTCore::UART_STOP_BITS_1);

    CANTramCore::initialize();

    randomStringLength = random(5,49);
    generateRandomString(randomBuffer,randomStringLength);
    bool result = uartInterface->send(randomBuffer,randomStringLength);
    TEST_ASSERT_TRUE(result);

}

void test_busmodule_receive_data_uart(){
    CANTramCore::attachModule(&busmodule);
    UARTInterface* uartInterface = busmodule.getInterfaces()[0]->as<UARTInterface*>();
    DEBUG_PRINTLN("Setting UART Paramters...");
    uartInterface->setBaudrate(115200);
    uartInterface->setDataBits(UARTCore::UART_DATA_8_BITS);
    uartInterface->setParity(UARTCore::UART_PARITY_NONE);
    uartInterface->setStopBits(UARTCore::UART_STOP_BITS_1);

    CANTramCore::initialize();

    randomStringLength = random(5,49);
    generateRandomString(randomBuffer,randomStringLength);
    uartInterface->send(randomBuffer,randomStringLength);

    INSTRUCTION_PRINTLN("This test fails if RX and TX are not connected!");

    char buffer[50];
    uartInterface->read(buffer);
    TEST_ASSERT_EQUAL_STRING(randomBuffer,buffer);
    
}


/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    //RUN_TEST(test_busmodule_check_for_mainmodule);
    RUN_TEST(test_busmodule_attachModule);
    RUN_TEST(test_busmodule_initialize);
    RUN_TEST(test_busModule_get_interfaces);
    RUN_TEST(measurment_busmodule_loopDuration);
    //RUN_TEST(test_busmodule_check_usage_of_hardware);
    //RUN_TEST(test_busmodule_send_data_uart);
    //RUN_TEST(test_busmodule_receive_data_uart);
    UNITY_END();
}

void loop(){

}