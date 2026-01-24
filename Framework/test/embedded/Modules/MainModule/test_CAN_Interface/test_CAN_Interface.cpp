#include <Arduino.h>
#include <unity.h>
#include "Debug.h"

#include "ESP32_CANCore.h"
#include "CANCore.h"

#define CAN_TX GPIO_NUM_17
#define CAN_RX GPIO_NUM_16

ESP32_CANCore canCore;
//Runs before tests
void setUp(){

}

//Runs after tests
void tearDown(){
    canCore.reset();
}

void test_CANCore_set_parameters(){
    DEBUG_PRINTLN("TEST: test_CANCore_set_parameters");
    bool result = canCore.setBaudrate(CANCore::BR_500K);
    TEST_ASSERT_TRUE(result);
    result = canCore.setPins(CAN_TX, CAN_RX);
    TEST_ASSERT_TRUE(result);
}

void test_CANCore_begin_end(){
    DEBUG_PRINTLN("TEST: test_CANCore_begin_end");
    bool result = canCore.setBaudrate(CANCore::BR_500K);
    TEST_ASSERT_TRUE(result);
    result = canCore.setPins(CAN_TX, CAN_RX);
    TEST_ASSERT_TRUE(result);
    result = canCore.begin();
    TEST_ASSERT_TRUE(result);
    delay(500);
    result = canCore.end();
    TEST_ASSERT_TRUE(result);
}

void test_CANCore_send_message(){
    DEBUG_PRINTLN("TEST: test_CANCore_send_message");
    bool result = canCore.setBaudrate(CANCore::BR_500K);
    TEST_ASSERT_TRUE(result);
    result = canCore.setPins(CAN_TX, CAN_RX);
    TEST_ASSERT_TRUE(result);
    result = canCore.begin();
    TEST_ASSERT_TRUE(result);

    CANCore::CANMessage message;
    
    message.isExtended = true;
    message.isRemote = false;
    message.length = 8;
    for(uint8_t i=0; i<8; i++){
        message.data[i] = i;
    }

    for(int i=0; i<5; i++){
        message.id = random(0x000, 0x7FF);
        message.data[0] = i;
        result = canCore.sendMessage(message);
        delay(100);
        TEST_ASSERT_TRUE(result);
    }
    

    
}
void test_CANCore_read_message(){
    DEBUG_PRINTLN("TEST: test_CANCore_read_message");
    DEBUG_PRINTLN("TEST: test_CANCore_send_message");
    bool result = canCore.setBaudrate(CANCore::BR_500K);
    TEST_ASSERT_TRUE(result);
    result = canCore.setPins(CAN_TX, CAN_RX);
    TEST_ASSERT_TRUE(result);
    result = canCore.begin();
    TEST_ASSERT_TRUE(result);

    DEBUG_PRINTLN("Your participation is needed! Send a message via a CAN Viewer like PCAN or InnoMakerUSB2CAN");
    DEBUG_PRINTLN("Waiting for messages...");
    result = false;
    CANCore::CANMessage message;
    while(!result){
        if(canCore.available() == 0){
            delay(100);
            DEBUG_PRINT(".");
            continue;
        }
        result = canCore.readMessage(message);
    }
    DEBUG_PRINTLN("Received message: " + message.toString());
    TEST_ASSERT_TRUE(result);

}

/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_CANCore_set_parameters);
    RUN_TEST(test_CANCore_begin_end);
    RUN_TEST(test_CANCore_send_message);
    RUN_TEST(test_CANCore_read_message);
    //RUN_TEST(test_function);
    UNITY_END();
}

void loop(){

}