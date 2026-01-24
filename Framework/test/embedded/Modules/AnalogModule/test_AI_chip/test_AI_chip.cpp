#include <Arduino.h>
#include <unity.h>
#include "../test/CANTramTestSetup.h"
#include "MAX22531.h"
#include "MAX22531_registers.h"

MAX22531 aiChip;
int csPin = ANALOGMODULE_AI_CS_PIN;

// Runs before tests
void setUp()
{
    setupSPITestEnvironment();
    aiChip = MAX22531();
    aiChip.setCSPin(csPin);
}

// Runs after tests
void tearDown()
{
    aiChip.HWreset();
    SPI.end();
    aiChip.~MAX22531();
}

void test_max22531_begin()
{
    bool result = aiChip.begin();
    TEST_ASSERT_TRUE(result); // Ensure initialization was successful
}

void test_max22531_readRegister()
{
    bool result = aiChip.begin();
    TEST_ASSERT_TRUE(result); // Ensure initialization was successful

    uint8_t address = MAX22531_REG_ID; // Example register address
    uint16_t data;

    // Read back from the register
    DEBUG_PRINTLN("Reading back from register 0x" + String(address, HEX) + " (PROD_ID)");
    uint8_t readResult = aiChip.readRegister(address, &data);
    DEBUG_PRINTLN("Data read: 0x" + String(data, HEX));
    TEST_ASSERT_EQUAL(MAX22531::STATUS_OK, readResult); // Ensure read operation was successful
}

void test_max22531_writeRegister()
{
    bool result = aiChip.begin();
    TEST_ASSERT_TRUE(result); // Ensure initialization was successful

    uint16_t readData;
    uint8_t prodIdAddress = MAX22531_REG_ID; // PROD_ID register address

    // Clear POR bit in CONTROL register
    uint8_t controlAddress = MAX22531_REG_CONTROL; // CONTROL register address
    uint16_t controlData = 0x0004;                 // Clear POR bit
    DEBUG_PRINTLN("Clearing POR bit in CONTROL register");
    uint8_t writeResult = aiChip.writeRegister(controlAddress, controlData);
    TEST_ASSERT_EQUAL(MAX22531::STATUS_OK, writeResult); // Ensure write operation was successful

    // Read PROD_ID to check if POR bit was cleared (it should not affect PROD_ID)
    DEBUG_PRINTLN("Reading PROD_ID Expecting 0x0001 or 0b00000000 00000001");
    uint8_t readResult = aiChip.readRegister(prodIdAddress, &readData);
    DEBUG_PRINTLN("PROD_ID read: 0x" + String(readData, HEX) + " or 0b" + String(readData, BIN));
    TEST_ASSERT_EQUAL(MAX22531::STATUS_OK, readResult); // Ensure read operation was
    TEST_ASSERT_EQUAL(1, readData);                     // PROD_ID should still be 0x0081
}

void test_max22531_read_channel()
{
    DEBUG_PRINTLN("ATTETION: This test requires that channel 4 (index=3) of the chip is connected to a 3,3V rail.");
    uint8_t result = aiChip.begin();
    TEST_ASSERT_TRUE(result); // Ensure initialization was successful
    delay(5000);              // Wait for the chip to stabilize
    uint16_t readData;

    result = aiChip.readChannel(3, &readData, true); // using index 3 as channel 4
    DEBUG_PRINTLN("Channel 4 read: " + String(readData));
    delay(100);

    TEST_ASSERT_EQUAL(MAX22531::STATUS_OK, result); // Ensure read operation was successful
    uint16_t expectedValue = 1351;                  // Expected value for 3.3V input on a 12-bit ADC
    uint16_t tolerance = 100;                       // Allowable tolerance
    CHIP_PRINTLN("Expecting around: " + String(expectedValue) + " +/- " + String(tolerance));
    TEST_ASSERT_UINT16_WITHIN(tolerance, expectedValue, readData); // Check if read value is within tolerance
}

void test_max22531_burstRead()
{
    DEBUG_PRINTLN("ATTETION: This test requires that channel 4 (index=3) of the chip is connected to a 3,3V rail.");
    uint8_t result = aiChip.begin();
    TEST_ASSERT_TRUE(result); // Ensure initialization was successful

    // Perform burst read starting from the first ADC register
    MAX22531::BurstResponse response = aiChip.burstRead(true);

    // Check if the burst read was successful
    TEST_ASSERT_EQUAL(MAX22531::STATUS_OK, response.status);

    // Print out the ADC values for verification
    DEBUG_PRINTLN("Burst Read Results:");
    DEBUG_PRINTLN("ADC1: " + String(response.adc1));
    DEBUG_PRINTLN("ADC2: " + String(response.adc2));
    DEBUG_PRINTLN("ADC3: " + String(response.adc3));
    DEBUG_PRINTLN("ADC4: " + String(response.adc4));
    DEBUG_PRINTLN("Interrupt Status: " + String(response.interrupt,BIN));
    DEBUG_PRINTLN("CRC: " + String(response.crc));

    // Basic validation of ADC values (assuming they should be within 0-4095 for a 12-bit ADC)
    TEST_ASSERT_UINT16_WITHIN(4095, 0, response.adc1);
    TEST_ASSERT_UINT16_WITHIN(4095, 0, response.adc2);
    TEST_ASSERT_UINT16_WITHIN(4095, 0, response.adc3);

    DEBUG_PRINTLN("Checking only channel 4 ...");
    uint16_t expectedValue = 1351;                  // Expected value for 3.3V input on a 12-bit ADC
    uint16_t tolerance = 100;                       // Allowable tolerance
    DEBUG_PRINTLN("Expecting around: " + String(expectedValue) + " +/- " + String(tolerance));
    TEST_ASSERT_UINT16_WITHIN(tolerance, expectedValue, response.adc4); // Check if read value is within tolerance

    
}
/* Write a function for each test*/

// Run tests
void setup()
{
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_max22531_begin);
    RUN_TEST(test_max22531_readRegister);
    RUN_TEST(test_max22531_writeRegister);
    RUN_TEST(test_max22531_read_channel);
    RUN_TEST(test_max22531_burstRead);
    UNITY_END();
}

void loop()
{
}