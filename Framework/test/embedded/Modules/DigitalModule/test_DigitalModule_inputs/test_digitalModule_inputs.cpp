#include <Arduino.h>
#include <unity.h>
#include "../test/CANTramTestSetup.h"
#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "DigitalModuleV1_0.h"
#include "../test/CANTramTestSetup.h"

#define OUTPUT_MASK 0xF0 //Bitmask for flexible IOs (1=Output,0=Input)
MainModuleV1_0 mainModule;
DigitalModuleV1_0 digitalModule(OUTPUT_MASK);

Interface* interfaces[12];

uint8_t outputValues = 0b00000000; //Initial output values

//Runs before tests
void setUp(){
    CANTramCore::attachModule(&mainModule);
    //setupSPITestEnvironment();
    
}

//Runs after tests
void tearDown(){
    CANTramCore::reset();
    outputValues = 0b00000000;
}

void test_digitalModule_attach(){
    bool result = CANTramCore::attachModule(&digitalModule);
    TEST_ASSERT_TRUE(result);

}

void testdigitalModule_initialize(){
    bool attachResult = CANTramCore::attachModule(&digitalModule);
    TEST_ASSERT_TRUE(attachResult);
    bool initResult = CANTramCore::initialize();
    TEST_ASSERT_TRUE(initResult);
    delay(50);
    //check if the CS Pins are correctly initialized (HIGH)
    digitalModule.getGPIOStart();
    OutputDefinition* csOutput = CANTramCore::getOutputDefinition(digitalModule.getGPIOStart());
    TEST_ASSERT_NOT_NULL(csOutput);
    OutputProvider* provider = csOutput->provider;
    TEST_ASSERT_NOT_NULL(provider);
    bool csState = provider->getOutput(csOutput);
    TEST_ASSERT_EQUAL(HIGH,csState);

    OutputDefinition* csInput = CANTramCore::getOutputDefinition(digitalModule.getGPIOStart()+1);
    TEST_ASSERT_NOT_NULL(csInput);
    provider = csInput->provider;
    TEST_ASSERT_NOT_NULL(provider);
    csState = provider->getOutput(csInput);
    TEST_ASSERT_EQUAL(HIGH,csState);
}

void test_digitalModule_interfaces(){
    bool attachResult = CANTramCore::attachModule(&digitalModule);
    TEST_ASSERT_TRUE(attachResult);
    bool initResult = CANTramCore::initialize();
    TEST_ASSERT_TRUE(initResult);
    delay(50);

    //get Array of interfaces
    INFO_PRINTLN("Getting interfaces...");
    for(int i =0;i<12;i++){
        INFO_PRINTLN("Getting interface "+String(i));
        interfaces[i] = digitalModule.getInterfaces()[i];
        TEST_ASSERT_NOT_NULL(interfaces[i]);
    }

    INFO_PRINTLN("Checking interfaces...");
    int i=0;
    //check inputs
    for (; i < 4; i++) {
        INFO_PRINTLN("Check index " + String(i));
        TEST_ASSERT_EQUAL(Interface::DIGITAL_INPUT, interfaces[i]->getType());
        TEST_ASSERT_TRUE(interfaces[i]->isValid());
    }
    //check flexible IOs
    for (;i<8;i++){
        INFO_PRINTLN("Check index " + String(i));
        if(OUTPUT_MASK & (1 << (i-4))){
            TEST_ASSERT_EQUAL(Interface::DIGITAL_OUTPUT,interfaces[i]->getType());
            TEST_ASSERT_TRUE(interfaces[i]->isValid());
        } else {
            TEST_ASSERT_EQUAL(Interface::DIGITAL_INPUT,interfaces[i]->getType());
            TEST_ASSERT_TRUE(interfaces[i]->isValid());
        }
    }
    //check outputs
    for(;i<12;i++){
        INFO_PRINTLN("Check index " + String(i));
        TEST_ASSERT_EQUAL(Interface::DIGITAL_OUTPUT,interfaces[i]->getType());
        TEST_ASSERT_TRUE(interfaces[i]->isValid());
    }
}

void measurement_digitalModule_loopDuration(){
    bool attachResult = CANTramCore::attachModule(&digitalModule);
    TEST_ASSERT_TRUE(attachResult);
    bool initResult = CANTramCore::initialize();
    TEST_ASSERT_TRUE(initResult);

    const uint8_t MEASUREMENTS = 100;
    MeasurementArray<MEASUREMENTS> durations;

    digitalModule.getInterfaces()[11]->setQ(HIGH); //Ensure interfaces are initialized
    //Measure loop duration
    for(uint8_t i=0;i<MEASUREMENTS;i++){
        uint32_t startTime = esp_timer_get_time();
        CANTramCore::loop();
        uint32_t endTime = esp_timer_get_time();
        durations.addSample(endTime - startTime);
    }

    MEASUREMENT_PRINTLN("Measured DigitalModuleV1_0::Cycle() Duration over " + String(MEASUREMENTS) + " cycles:");
    MEASUREMENT_PRINTLN("  Average duration: " + String(durations.getAverage()) + " us");
    MEASUREMENT_PRINTLN("  Max duration: " + String(durations.getMax()) + " us");
    MEASUREMENT_PRINTLN("  Min duration: " + String(durations.getMin()) + " us");
    TEST_ASSERT_LESS_OR_EQUAL(2000, durations.getMax()); // Ensure max duration is less than 2ms
}
void test_digitalModule_readInputs(){
    bool attachResult = CANTramCore::attachModule(&digitalModule);
    TEST_ASSERT_TRUE(attachResult);
    bool initResult = CANTramCore::initialize();
    TEST_ASSERT_TRUE(initResult);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    //Get interfaces
    INFO_PRINTLN("Getting interfaces...");
    for(int i =0;i<12;i++){
        INFO_PRINTLN("Getting interface "+String(i));
        interfaces[i] = digitalModule.getInterfaces()[i];
        TEST_ASSERT_NOT_NULL(interfaces[i]);
    }

    //Read inputs
    uint8_t readValues = 0;
    for(int i=0;i<12;i++){
        //check if interface is input
        if(interfaces[i]->getType() != Interface::DIGITAL_INPUT){
            INFO_PRINTLN("Skipping interface "+String(i)+" as it is not an input");
            continue; //Skip if not an input
        }

        //Check high state
        INSTRUCTION_PRINTLN("Connect input "+interfaces[i]->getName()+" to 24V");
        while(!interfaces[i]->getQ()){
            CANTramCore::loop(); //Run a loop to update inputs
            vTaskDelay(2000 / portTICK_PERIOD_MS); //Wait a bit
        }
        INSTRUCTION_PRINTLN("Disconnect input "+interfaces[i]->getName()+" from 24V");
        
        //Check low state
        while(interfaces[i]->getQ()){
            CANTramCore::loop(); //Run a loop to update inputs
            vTaskDelay(2000 / portTICK_PERIOD_MS); //Wait a bit
        }
        INFO_PRINTLN("INPUT "+interfaces[i]->getName()+" tested successfully.");
    }
    
}


/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_digitalModule_attach);
    RUN_TEST(testdigitalModule_initialize);
    RUN_TEST(test_digitalModule_interfaces);
    RUN_TEST(measurement_digitalModule_loopDuration);
    //RUN_TEST(test_digitalModule_readInputs);
    UNITY_END();
}

void loop(){

}