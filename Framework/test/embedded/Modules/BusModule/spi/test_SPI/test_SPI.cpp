#include <Arduino.h>
#include <unity.h>

#include <TestingUtility.h>
#include "BusModule.h"
#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "Debug.h"
#include "UARTInterface.h"
#include "Interface.h"
#include "../test/CANTramTestSetup.h"
#include "ISO1H816G.h"

MainModuleV1_0 mainmodule;
BusModuleV1_0 busmodule;
ISO1H816G dqChip;

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
}

/* Write a function for each test*/
void test_BusModule_external_SPI_device()
{
    DEBUG_PRINTLN("Running test_BusModule_external_SPI_device");
    CANTramCore::attachModule(&busmodule);
    INSTRUCTION_PRINTLN("Add the DQ-Prototype board to the SPI Interface of the Busmodule! Otherwise the test will fail.");
    bool addResult = busmodule.addSPIDevice(&dqChip, true); // Add the SPI device to the BusModuleV1_0
    dqChip.setSPISettings(1000000, MSBFIRST, SPI_MODE3); // Set SPI settings for the device
    TEST_ASSERT_TRUE(addResult); // Verify that the SPI device was added successfully

    bool initResult = CANTramCore::initialize(); // Initialize the core and modules
    TEST_ASSERT_TRUE(initResult); // Verify that initialization was successful
    
    uint8_t beginResult = dqChip.begin(); // Begin communication with the device
    TEST_ASSERT_TRUE(beginResult == ISO1H816G::STATUS_OK); // Verify that the device began successfully

    //Set and read outputs
    uint8_t randomOutpus = (random(0, 256) | 1); // Generate a random output value, bit 0 is always 1
    uint8_t response = dqChip.setOutputs(randomOutpus); // Set all outputs high
    TEST_ASSERT_EQUAL(response, ISO1H816G::STATUS_OK); // Verify that setting outputs was successful
    vTaskDelay(3000 / portTICK_PERIOD_MS); // Wait for the device to process

    response = dqChip.setOutputs(0); // deactivate all outputs
    TEST_ASSERT_EQUAL(response, ISO1H816G::STATUS_OK); // Verify that setting outputs was successful
   

}

//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_BusModule_external_SPI_device);
    UNITY_END();
}

void loop(){

}