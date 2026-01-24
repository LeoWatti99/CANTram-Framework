#include <Arduino.h>
#include <unity.h>

#include "Adafruit_MCP23X17.h"
#include "Wire.h"
#include "Debug.h"

Adafruit_MCP23X17 shiftRegister;

#define SDA_PIN GPIO_NUM_13
#define SCL_PIN GPIO_NUM_14
#define I2C_ADDRESS_SHIFTREGISTER 0b100111
//Runs before tests
void setUp(){

}

//Runs after tests
void tearDown(){
    Wire.end();
    DEBUG_PRINTLN("I2C Closed");
}

void test_start_i2c(){
    DEBUG_PRINTLN("[TEST_Shiftregister] test_start_i2c");
    bool result = true;
    result &= Wire.setPins(SDA_PIN,SCL_PIN);
    TEST_ASSERT_TRUE(result);
    delay(10);
    result &= Wire.begin();
    TEST_ASSERT_TRUE(result);
    result &= Wire.setClock(400000);
    TEST_ASSERT_TRUE(result);
}

void test_start_shiftregister(){
    DEBUG_PRINTLN("[TEST_Shiftregister] test_start_shiftregister");
    bool result = true;
    result &= Wire.setPins(SDA_PIN,SCL_PIN);
    TEST_ASSERT_TRUE(result);
    result &= Wire.begin();
    TEST_ASSERT_TRUE(result);
    result &= Wire.setClock(400000);
    TEST_ASSERT_TRUE(result);

    result &= shiftRegister.begin_I2C(I2C_ADDRESS_SHIFTREGISTER, &Wire);
    TEST_ASSERT_TRUE(result);
    DEBUG_PRINTLN("Shift Register Initialized");
}





/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    DEBUG_PRINTLN("[TEST_Shiftregister] Starting...");
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_start_i2c);
    RUN_TEST(test_start_shiftregister);
    UNITY_END();
}

void loop(){

}