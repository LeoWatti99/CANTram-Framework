#include <Arduino.h>
#include <unity.h>

#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "RelaisModuleV1_0.h"
#include "RelaisInterface.h"
#include "../test/CANTramTestSetup.h"

MainModuleV1_0 mainModule;
RelaisModuleV1_0 relaisModule;
RelaisInterface* relaisInterfaces[4];

//Runs before tests
void setUp(){
    CANTramCore::attachModule(&mainModule);
}

//Runs after tests
void tearDown(){
    CANTramCore::reset();
}

void test_relaisModule_attach(){
    bool result = CANTramCore::attachModule(&relaisModule);
    TEST_ASSERT_TRUE(result);

}

void test_relaisModule_init(){
    bool result = CANTramCore::attachModule(&relaisModule);
    TEST_ASSERT_TRUE(result);
    result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);
}

void test_relaisModule_get_interfaces(){
    bool result = CANTramCore::attachModule(&relaisModule);
    TEST_ASSERT_TRUE(result);
    Interface** interfaces = relaisModule.getInterfaces();
    TEST_ASSERT_NOT_NULL(interfaces);
    for(int i=0;i<relaisModule.getInterfaceCount();i++){
        TEST_ASSERT_NOT_NULL(interfaces[i]);
        relaisInterfaces[i] = interfaces[i]->as<RelaisInterface*>();
        TEST_ASSERT_NOT_NULL(relaisInterfaces[i]);
        TEST_ASSERT_TRUE(relaisInterfaces[i]->isValid());
    }
}

void measure_relaisModule_loopDuration(){
    bool result = CANTramCore::attachModule(&relaisModule);
    TEST_ASSERT_TRUE(result);
    result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);

    relaisModule.getInterfaces()[0]->setQ(HIGH); //Ensure interfaces are initialized

    const uint8_t MEASUREMENTS = 100;
    MeasurementArray<MEASUREMENTS> durations;

    //Measure loop duration
    for(uint8_t i=0;i<MEASUREMENTS;i++){
        uint32_t startTime = esp_timer_get_time();
        CANTramCore::loop();
        uint32_t endTime = esp_timer_get_time();
        durations.addSample(endTime - startTime);
    }

    MEASUREMENT_PRINTLN("Measured RelaisModuleV1_0::Cycle() Duration over " + String(MEASUREMENTS) + " cycles:");
    MEASUREMENT_PRINTLN("  Average duration: " + String(durations.getAverage()) + " us");
    MEASUREMENT_PRINTLN("  Max duration: " + String(durations.getMax()) + " us");
    MEASUREMENT_PRINTLN("  Min duration: " + String(durations.getMin()) + " us");
    TEST_ASSERT_LESS_OR_EQUAL(2000, durations.getMax()); // Ensure max duration is less than 2ms
}
void test_relaisModule_set_relais(){
    DEBUG_PRINTLN("Starting relais module test...");
    bool result = CANTramCore::attachModule(&relaisModule);
    TEST_ASSERT_TRUE(result);
    Interface** interfaces = relaisModule.getInterfaces();
    TEST_ASSERT_NOT_NULL(interfaces);

    gpio_set_direction(GPIO_NUM_34, GPIO_MODE_INPUT); //Set INT pin as input
    DEBUG_PRINTLN("INT pin set to input.");

    //Check all interfaces and reset relais
    for(int i=0;i<relaisModule.getInterfaceCount();i++){
        TEST_ASSERT_NOT_NULL(interfaces[i]);
        relaisInterfaces[i] = interfaces[i]->as<RelaisInterface*>();
        TEST_ASSERT_NOT_NULL(relaisInterfaces[i]);
        TEST_ASSERT_TRUE(relaisInterfaces[i]->isValid());
        relaisInterfaces[i]->setQ(LOW); //Set all relais to LOW
        CANTramCore::loop(); //Call loop to apply changes
    }

    //Check correct operation of each relais
    for(int i=0;i<relaisModule.getInterfaceCount();i++){
        
        
        DEBUG_PRINTLN("Testing relais interface " + String(i)+". Connect +3V3 to " + String(i+1) + "3 and GPIO_34 to +" + String(i+1) + "4 of the relais module.");
        
        //Set relais ON
        relaisInterfaces[i]->setQ(HIGH);
        CANTramCore::loop(); //Call loop to apply changes
        vTaskDelay(500/portTICK_PERIOD_MS); //Wait for relais to settle
        
        DEBUG_PRINTLN("Waiting for INT pin to go HIGH...");
        result=false;
        bool lastRead=false;
        
        while(!result){
            bool current = digitalRead(GPIO_NUM_34);
            result = lastRead && current;
            lastRead = current;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            
        } //Wait for HIGH on input


        //Set relais OFF
        relaisInterfaces[i]->setQ(LOW);
        CANTramCore::loop(); //Call loop to apply changes
        vTaskDelay(500/portTICK_PERIOD_MS); //Wait for relais to settle

        DEBUG_PRINTLN("Waiting for INT pin to go HIGH...");
        result=false;
        lastRead=false;

        while(!result){
            bool current = digitalRead(GPIO_NUM_34);
            result = !lastRead && !current;
            lastRead = current;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            
        } //Wait for LOW on input
    }
}

/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    delay(1000);
    UNITY_BEGIN();
    RUN_TEST(test_relaisModule_attach);
    RUN_TEST(test_relaisModule_init);
    RUN_TEST(test_relaisModule_get_interfaces);
    RUN_TEST(measure_relaisModule_loopDuration);
    //RUN_TEST(test_relaisModule_set_relais);
    UNITY_END();
}

void loop(){

}