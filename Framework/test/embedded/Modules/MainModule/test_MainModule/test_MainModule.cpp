
#include <Arduino.h>
#include <unity.h>
#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "Debug.h"
#include "../test/CANTramTestSetup.h"
#include "DigitalModuleV1_0.h"
#include "RelaisModuleV1_0.h"
#include "AnalogModuleV1_0.h"
#include "BusModule.h"

constexpr uint8_t OUTPUT_MASK = 0b11110011;
DigitalModuleV1_0 digitalModule(OUTPUT_MASK);
DigitalInput* di8;
DigitalInput* di3;
DigitalOutput* dq1;
DigitalOutput* dq5;

//Interfaces entsprechend der OUTPUT_MASK
//Instanzerstellen
MainModuleV1_0 mainModule;
AnalogModuleV1_0 analogModule;
RelaisModuleV1_0 relaisModule1;
RelaisModuleV1_0 relaisModule2;
BusModuleV1_0 busModule;

UARTInterface* uartInterface;
I2CInterface* i2cInterface;


AnalogInput* aiPotentiometer;
AnalogOutput* aq1;
AnalogOutput* aoLampeHinten;
RelaisInterface* rq1;
RelaisInterface* rqVentilBremse;
CANInterface* canInterface;
//Runs before tests
void setUp(){
    
    
}

//Runs after tests
void tearDown(){
    CANTramCore::reset();
}

/* Write a function for each test*/
void test_MainModule_attach(){
    DEBUG_PRINTLN("TEST: test_MainModule_Attach");
    bool result = CANTramCore::attachModule(&mainModule);
    TEST_ASSERT_TRUE(result);
}
void test_MainModule_Init(){
    DEBUG_PRINTLN("TEST: test_MainModule_Init");
    CANTramCore::attachModule(&mainModule);
    bool result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);
}

void test_MainModule_getInterfaces(){
    DEBUG_PRINTLN("TEST: test_MainModule_getInterfaces");
    CANTramCore::attachModule(&mainModule);
    bool result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);

    //Check interface count
    uint8_t interfaceCount = mainModule.getInterfaceCount();
    DEBUG_PRINTLN("MainModuleV1_0 provides "+String(interfaceCount)+" interfaces");
    TEST_ASSERT_EQUAL_UINT8(1, interfaceCount);
    
    //Check interface type
    CANInterface* canInterface = mainModule.getInterfaces()[0]->as<CANInterface*>();
    TEST_ASSERT_NOT_NULL(canInterface);
    Interface::Type type = canInterface ->getType();
    TEST_ASSERT_EQUAL_INT(Interface::BUS, type);
    
    
}

void measure_MainModule_loopDuration(){
    DEBUG_PRINTLN("TEST: test_MainModule_loopDuration");
    CANTramCore::attachModule(&mainModule);
    CANTramCore::initialize();

    //Prepare measurement
    const uint8_t MEASUREMENTS = 100;
    MeasurementArray<MEASUREMENTS> durations;

    //Measure loop duration
    for(uint8_t i=0;i<MEASUREMENTS;i++){
        uint32_t startTime = esp_timer_get_time();
        CANTramCore::loop();
        uint32_t endTime = esp_timer_get_time();
        durations.addSample(endTime - startTime);
    }
    
    MEASUREMENT_PRINTLN("Measured CANTramCore::loop() durations over "+String(MEASUREMENTS)+" cycles:");
    MEASUREMENT_PRINTLN("  Average duration: "+String(durations.getAverage())+" us");
    MEASUREMENT_PRINTLN("  Max duration: "+String(durations.getMax())+" us");
    MEASUREMENT_PRINTLN("  Min duration: "+String(durations.getMin())+" us");
    TEST_ASSERT_LESS_OR_EQUAL(1000, durations.getMax()); //Ensure max duration is less than 1ms
    
    
}
void test_MainModule_Temperature(){
    DEBUG_PRINTLN("TEST: test_MainModule_Temperature");
    CANTramCore::attachModule(&mainModule);
    bool result = CANTramCore::initialize();
    uint16_t temperature = mainModule.getTemperatureCelsius();
    uint16_t expected = 200; // 20°C
    uint16_t tolerance = 100; // +/- 10.0°C
    MEASUREMENT_PRINTLN("Measured temperature: "+String(temperature/10.0f)+" °C");
    MEASUREMENT_PRINTLN("Expected temperature: "+String(expected/10.0f)+" °C +/- "+String(tolerance/10.0f)+" °C");
    TEST_ASSERT_UINT16_WITHIN(tolerance, expected, temperature);
}

void test_MainModule_Expansion_Bus(){
    DEBUG_PRINTLN("TEST: test_MainModule_Expansion_Bus");
    bool result = CANTramCore::initialize();
    TEST_ASSERT_TRUE(result);

    //setup test input
    pinMode(GPIO_NUM_34, INPUT);
    DEBUG_PRINTLN("Using GPIO_NUM_34 as input for expansion bus test. Please connect it to GND via a 10kOhm pulldown resistor and make sure no modul except a debugmodule is attached to the mainmodule.");
    
    //Test each expansion bus output
    uint8_t outPutCount = CANTramCore::getProvidedGPIOs();
    for(uint8_t i=0; i<outPutCount; i++){
        OutputDefinition* def = CANTramCore::getOutputDefinition(i);
        result = mainModule.setOutput(def, LOW);
        TEST_ASSERT_TRUE(result);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    DEBUG_PRINTLN("Resetted all outputs to LOW");
    DEBUG_PRINTLN("MainModuleV1_0 provides "+String(outPutCount)+" GPIOs");
    for(uint8_t i=0; i<outPutCount; i++){
        OutputDefinition* def = CANTramCore::getOutputDefinition(i);
        TEST_ASSERT_NOT_NULL(def); //Check definition exists
        DEBUG_PRINTLN("Testing output "+def->toString());
        //Configure as output
        result = mainModule.configureOutput(def, OUTPUT);
        TEST_ASSERT_TRUE(result);
        //Set output HIGH
        result = mainModule.setOutput(def, HIGH);
        TEST_ASSERT_TRUE(result);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        //Wait for user to measure output
        DEBUG_PRINTLN("Action Required: Please measure output voltage of "+def->toString()+" and verify it is HIGH");
        DEBUG_PRINTLN("Connect "+def->toString()+ "to GPIO_NUM_34 to continue the test");

        result=false;
        bool lastRead=false;
        
        while(!result){
            bool current = digitalRead(GPIO_NUM_34);
            result = lastRead && current;
            lastRead = current;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            
        } //Wait for HIGH on input
        
        //Set output LOW
        result = mainModule.setOutput(def, LOW);
        TEST_ASSERT_TRUE(result);
        while(!result){
            bool current = digitalRead(GPIO_NUM_34);
            result = !lastRead && !current;
            lastRead = current;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            
        } //Wait for LOW on input
        vTaskDelay(100 / portTICK_PERIOD_MS);

        
        
        //continue to next output
        
    }
}

//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    
    #ifdef TARGET_ESP32
    //RUN_TEST(test_MainModule_attach);
    //RUN_TEST(test_MainModule_Init);
    //RUN_TEST(test_MainModule_getInterfaces);
    //RUN_TEST(measure_MainModule_loopDuration);
    #endif
    #ifdef TARGET_MAINMODULE
    RUN_TEST(test_MainModule_Temperature);
    //RUN_TEST(test_MainModule_Expansion_Bus);
    #endif
    UNITY_END();
}

void loop(){

}