/**
 * @file test_AnalogModule.cpp
 * @author your name (you@domain.com)
 * @brief Tests the AnalogModuleV1_0 in combination with a MainmoduleV1_0
 * @version 0.1
 * @date 2025-10-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <Arduino.h>
#include <unity.h>
#include "CANTramCore.h"
#include "MainModuleV1_0.h"
#include "AnalogModuleV1_0.h"
#include "Debug.h"
#include "../test/CANTramTestSetup.h"
#include "Interface.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"

MainModuleV1_0 mainModule;
AnalogModuleV1_0 analogModule;

constexpr uint8_t MULTISAMPLE_COUNT = 10;
struct Multisample{
            uint16_t sample[MULTISAMPLE_COUNT];
            uint8_t index =0;
            uint16_t getAverage(){
                uint32_t sum =0;
                for(uint8_t i=0;i<index;i++){
                    sum += sample[i];
                }
                return sum / index;
            }
            uint16_t getMax(){
                uint16_t maxVal =0;
                for(uint8_t i=0;i<index;i++){
                    if(sample[i] > maxVal){
                        maxVal = sample[i];
                    }
                }
                return maxVal;
            }
            uint16_t getMin(){
                uint16_t minVal =4095;
                for(uint8_t i=0;i<index;i++){
                    if(sample[i] < minVal){
                        minVal = sample[i];
                    }
                }
                return minVal;
            }
            void addSample(uint16_t value){
                if(index >= MULTISAMPLE_COUNT){
                    ERROR_PRINTLN("Multisample overflow");
                    return;
                }
                    sample[index++] = value;  
            }
        };
        

// Runs before tests
void setUp()
{
    
    CANTramCore::attachModule(&mainModule);
    setupSPITestEnvironment();
}

// Runs after tests
void tearDown()
{
    CANTramCore::reset();
}


void test_analogModule_initialize()
{
    DEBUG_PRINTLN("TEST: test_analogModule_initialize");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully
}

void test_analogModule_interfaces()
{
    DEBUG_PRINTLN("TEST: test_analogModule_interfaces");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    DEBUG_PRINTLN("Checking AnalogModuleV1_0 interfaces");
    Interface **interfaces = analogModule.getInterfaces();
    size_t interfaceCount = analogModule.getInterfaceCount();
    TEST_ASSERT_NOT_NULL(interfaces);     // Ensure interfaces is not null
    TEST_ASSERT_EQUAL(8, interfaceCount); // Ensure there are 8 interfaces

    // Check that all interfaces are valid
    for (size_t i = 0; i < interfaceCount; i++)
    {
        DEBUG_PRINTLN("Checking interface " + String(i) + ": " + interfaces[i]->getName());
        TEST_ASSERT_NOT_NULL(interfaces[i]);           // Ensure interface is not null
        TEST_ASSERT_FALSE(interfaces[i]->isInvalid()); // Ensure interface is not invalid
        TEST_ASSERT_TRUE(interfaces[i]->isValid());    // Ensure interface is valid
        if (i < 4)
        {
            TEST_ASSERT_EQUAL(Interface::ANALOG_OUTPUT, interfaces[i]->getType()); // Ensure first 4 interfaces are analog outputs
        }
        else
        {
            TEST_ASSERT_EQUAL(Interface::ANALOG_INPUT, interfaces[i]->getType()); // Ensure last 4 interfaces are analog inputs
        }
    }
}

void measurement_analogModule_loopDuration()
{
    DEBUG_PRINTLN("TEST: test_analogModule_loopDuration");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    // Prepare measurement
    const uint8_t MEASUREMENTS = 100;
    MeasurementArray<MEASUREMENTS> durations;

    // Measure loop duration
    for (uint8_t i = 0; i < MEASUREMENTS; i++)
    {
        uint32_t start = micros();
        CANTramCore::loop();
        uint32_t end = micros();
        durations.addSample(end - start);
    }

    MEASUREMENT_PRINTLN("Measured CANTramCore::loop() durations over " + String(MEASUREMENTS) + " cycles:");
    MEASUREMENT_PRINTLN("  Average duration: " + String(durations.getAverage()) + " us");
    MEASUREMENT_PRINTLN("  Max duration: " + String(durations.getMax()) + " us");
    MEASUREMENT_PRINTLN("  Min duration: " + String(durations.getMin()) + " us");
    TEST_ASSERT_LESS_OR_EQUAL(2000, durations.getMax()); // Ensure max duration is less than 2ms
}

void test_analogModule_output()
{
    DEBUG_PRINTLN("TEST: test_analogModule_output");
    DEBUG_PRINTLN("Output AQ1 has to be connected to input AI1 for this test.");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    /*//TEMPORARY
    pinMode(SPI_CS_DI, OUTPUT);
    pinMode(SPI_CS_DQ, OUTPUT);
    pinMode(SPI_CS_AI, OUTPUT);
    digitalWrite(SPI_CS_DI, HIGH);
    digitalWrite(SPI_CS_DQ, HIGH);
    digitalWrite(SPI_CS_AI, HIGH);
    */

    DEBUG_PRINTLN("Checking AnalogModuleV1_0 output interfaces");
    Interface **interfaces = analogModule.getInterfaces();
    size_t interfaceCount = analogModule.getInterfaceCount();
    TEST_ASSERT_EQUAL(8, interfaceCount); // Ensure there are 8 interfaces

    //Get interface for AQ1
    DEBUG_PRINTLN("Getting interface AQ1");
    AnalogOutput *output0 = interfaces[0]->as<AnalogOutput *>();
    TEST_ASSERT_NOT_NULL(output0); // Ensure interface is an analog output

    // Set random value for analog outputs
    uint16_t outputValue = random(0, 4095);
    DEBUG_PRINTLN("Setting AQ1 to " + String(outputValue));
    output0->setQ(outputValue);
    TEST_ASSERT_EQUAL(outputValue, output0->getQ());

    // Run cycle to apply output values
    DEBUG_PRINTLN("Running module cycle to apply output values");
    analogModule.cycle(nullptr);
    delay(10);

    // Get the input interface for AI1
    DEBUG_PRINTLN("Reading back value from AI1");
    AnalogInput *input0 = interfaces[4]->as<AnalogInput *>();
    TEST_ASSERT_NOT_NULL(input0); // Ensure interface is an analog input

    // Run cycle again to read inputs
    constexpr uint8_t MAX_ATTEMPTS = 100;
    DEBUG_PRINTLN("Running module cycle to read input values");
    uint16_t inputValue =0;

    constexpr uint16_t tolerance = 100;
    uint16_t maxValue = outputValue+tolerance;
    uint16_t minValue = outputValue-tolerance;

    for (int i = 0; i < MAX_ATTEMPTS; i++)
    {
        analogModule.cycle(nullptr);
        
        for(int i=0;i<4;i++){
            DEBUG_PRINTLN("AI"+String(i+1)+" value: " + String(interfaces[4 + i]->getQ()));
            if(i==0){
                inputValue = interfaces[4 + i]->getQ();
            }
        }

        DEBUG_PRINTLN("Expected value for AI1: " + String(outputValue) + " +/- " + String(tolerance));
        if(inputValue >= minValue && inputValue <= maxValue) {
            break; // Value is within tolerance
        }
        
    }

    TEST_ASSERT_UINT16_WITHIN(tolerance, outputValue, inputValue); // Ensure input value is within 50 of expected value
}

void test_analogModule_output_controlled()
{
    DEBUG_PRINTLN("TEST: test_analogModule_output");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    /*//TEMPORARY
    pinMode(SPI_CS_DI, OUTPUT);
    pinMode(SPI_CS_DQ, OUTPUT);
    pinMode(SPI_CS_AI, OUTPUT);
    digitalWrite(SPI_CS_DI, HIGH);
    digitalWrite(SPI_CS_DQ, HIGH);
    digitalWrite(SPI_CS_AI, HIGH);
    */

    DEBUG_PRINTLN("Checking AnalogModuleV1_0 output interfaces");
    Interface **interfaces = analogModule.getInterfaces();
    size_t interfaceCount = analogModule.getInterfaceCount();
    TEST_ASSERT_EQUAL(8, interfaceCount); // Ensure there are 8 interfaces
    
    //Using extendbus pin 11 / GPIO_5 as control pin for testing
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
    INFO_PRINTLN("Using GPIO_5 as control pin for testing.");
    INSTRUCTION_PRINTLN("Connect button to GPIO_5 (EXTENDBUS pin 11) to GND and press to start the test.");

    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    INFO_PRINTLN("Button pressed, starting test.");

     // Testing each output individually
    for(int i=0;i<4;i++){
        INFO_PRINTLN("Testing output interface AQ"+String(i+1));

        //get the corresponding interface
        AnalogOutput *output = interfaces[i]->as<AnalogOutput *>();
        TEST_ASSERT_NOT_NULL(output); // Ensure interface is an analog output

        uint16_t currentValue =0;
        const uint16_t MAX_VALUE = 4095;
        const uint8_t STEP_COUNT = 20;
        const uint16_t stepSize = MAX_VALUE / STEP_COUNT;
        uint8_t step=0;
        INSTRUCTION_PRINTLN("Connect output AQ"+String(i+1)+" to oscilloscope and measure duty cycle. Press button to continue to next value.");
        for(currentValue = 0; currentValue<=MAX_VALUE;currentValue += stepSize){
            
            //set output value
            output->setQ(currentValue);
            TEST_ASSERT_EQUAL(currentValue, output->getQ());
            CANTramCore::loop(); //Run a loop to apply the value

            //print current step
            INFO_PRINTLN(String(++step)+ ": Setting AQ"+String(i+1)+" to " + String(currentValue));
            while(gpio_get_level(GPIO_NUM_5)){
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
            while(!gpio_get_level(GPIO_NUM_5)){
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
        }
        INFO_PRINTLN("Finished testing output interface AQ"+String(i+1));
        output->setQ(0);
        CANTramCore::loop(); //Run a loop to apply the value
        vTaskDelay(10/portTICK_PERIOD_MS);
        
    }
}
    

void test_analogModule_input_controlled(){
    DEBUG_PRINTLN("TEST: test_analogModule_input_controlled");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    DEBUG_PRINTLN("Checking AnalogModuleV1_0 input interfaces");
    Interface **interfaces = analogModule.getInterfaces();
    size_t interfaceCount = analogModule.getInterfaceCount();
    TEST_ASSERT_EQUAL(8, interfaceCount); // Ensure there are 8 interfaces
    
    //Using extendbus pin 11 / GPIO_5 as control pin for testing
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
    INFO_PRINTLN("Using GPIO_5 as control pin for testing.");
    INSTRUCTION_PRINTLN("Connect button to GPIO_5 (EXTENDBUS pin 11) to GND and press to start the test.");

    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    INFO_PRINTLN("Button pressed, starting test.");

     // Testing each input individually
    AnalogInput *inputInterface[4];
    inputInterface[0] = interfaces[4]->as<AnalogInput *>();
    inputInterface[1] = interfaces[5]->as<AnalogInput *>();
    inputInterface[2] = interfaces[6]->as<AnalogInput *>();
    inputInterface[3] = interfaces[7]->as<AnalogInput *>();

    // Set voltages
    const float MAX_VOLTAGE = 10;
    const uint8_t SAMPLE_COUNT = 20;
    const float STEP_SIZE = MAX_VOLTAGE / SAMPLE_COUNT;
    float currentVoltage = 0;

    

    for(int sampleNumber = 0 ; currentVoltage <= MAX_VOLTAGE ; sampleNumber++, currentVoltage += STEP_SIZE){
        INSTRUCTION_PRINTLN("Set voltage to " + String(currentVoltage,2) + "V and press button to continue.");
        while(gpio_get_level(GPIO_NUM_5)){
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
        while(!gpio_get_level(GPIO_NUM_5)){
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
        //take the measurments by multisampling
        
        
        Multisample aiXsamples[4];

        for (int i=0; i<MULTISAMPLE_COUNT; i++){
            
            //read input value
            CANTramCore::loop(); //Run a loop to read the value
            vTaskDelay(10/portTICK_PERIOD_MS);

            aiXsamples[0].addSample(inputInterface[0]->getQ());
            aiXsamples[1].addSample(inputInterface[1]->getQ());
            aiXsamples[2].addSample(inputInterface[2]->getQ());
            aiXsamples[3].addSample(inputInterface[3]->getQ());
        }
        //Print results for one input step  
        for(int i=0;i<4;i++){
            INFO_PRINTLN("AI"+String(i+1)+" samples: Min=" + String(aiXsamples[i].getMin()) + " Max=" + String(aiXsamples[i].getMax()) + " Avg=" + String(aiXsamples[i].getAverage()));
        }    
    }
}

void test_analogModule_input_automatic(){
    DEBUG_PRINTLN("TEST: test_analogModule_input_controlled");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    DEBUG_PRINTLN("Initializing CANTramCore");
    result = CANTramCore::initialize(); // Initializes all attached modules
    TEST_ASSERT_TRUE(result);           // Ensure core was initialized successfully

    DEBUG_PRINTLN("Checking AnalogModuleV1_0 input interfaces");
    Interface **interfaces = analogModule.getInterfaces();
    size_t interfaceCount = analogModule.getInterfaceCount();
    TEST_ASSERT_EQUAL(8, interfaceCount); // Ensure there are 8 interfaces

     // get each input individually
    AnalogInput *inputInterface[4];
    inputInterface[0] = interfaces[4]->as<AnalogInput *>();
    inputInterface[1] = interfaces[5]->as<AnalogInput *>();
    inputInterface[2] = interfaces[6]->as<AnalogInput *>();
    inputInterface[3] = interfaces[7]->as<AnalogInput *>();

    //get output interface
    AnalogOutput *outputInterface = interfaces[0]->as<AnalogOutput *>();
    outputInterface->setQ(2048); //Set to mid value
    CANTramCore::loop(); //Run a loop to apply the value
    
    INSTRUCTION_PRINTLN("Connect button to GPIO_5 (EXTENDBUS pin 11) to GND and press to start the test.");
    //Using extendbus pin 11 / GPIO_5 as control pin for testing
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
    
    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    INSTRUCTION_PRINTLN("Connect output AQ1 to RC Filter with 4,7k resistor and 10 uF capacitor to GND.");
    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    INSTRUCTION_PRINTLN("Connect filter output to all of the Inputs AI1-AI4.");
    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    INSTRUCTION_PRINTLN("Connect filter output to oscillcoscope to verify there are no voltage ripples left.");
    while(gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    while(!gpio_get_level(GPIO_NUM_5)){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }

    
    INFO_PRINTLN("Automatic test started, pls wait.");

    // Set voltages
    const float MAX_VOLTAGE = 10;
    const uint16_t SAMPLE_COUNT = 250;
    const float MAX_VALUE = 4095;
    uint16_t currentSample = 0;

    

    for(currentSample = 0 ; currentSample < SAMPLE_COUNT ; currentSample++){

        //set voltage
        outputInterface->setQ((currentSample * MAX_VALUE) / SAMPLE_COUNT);
        MEASUREMENT_PRINTLN("Current output value: " + String(outputInterface->getQ()));
        CANTramCore::loop(); //Run a loop to apply the value

        //let the filer settle
        vTaskDelay(700/portTICK_PERIOD_MS);
        
        //take the measurments by multisampling
        Multisample aiXsamples[4];
        for (int i=0; i<MULTISAMPLE_COUNT; i++){
            
            //read input value
            CANTramCore::loop(); //Run a loop to read the value
            vTaskDelay(10/portTICK_PERIOD_MS);

            aiXsamples[0].addSample(inputInterface[0]->getQ());
            aiXsamples[1].addSample(inputInterface[1]->getQ());
            aiXsamples[2].addSample(inputInterface[2]->getQ());
            aiXsamples[3].addSample(inputInterface[3]->getQ());
        }
        //Print results for one input step  
        for(int i=0;i<4;i++){
            MEASUREMENT_PRINTLN("AI"+String(i+1)+" samples: Min=" + String(aiXsamples[i].getMin()) + " Max=" + String(aiXsamples[i].getMax()) + " Avg=" + String(aiXsamples[i].getAverage()));
        }    
    }
}

//Einzelne Testfunktion
void test_analogModule_attach()
{
    DEBUG_PRINTLN("TEST: test_analogModule_attach");
    DEBUG_PRINTLN("Attaching AnalogModuleV1_0 to CANTramCore");
    bool result = CANTramCore::attachModule(&analogModule);
    TEST_ASSERT_TRUE(result); // Ensure module was attached successfully

    // Check which Pins are used by the AnalogModuleV1_0
    DEBUG_PRINTLN("Checking GPIO usage after attaching AnalogModuleV1_0");
    uint8_t usedGPIOs = analogModule.getGPIODemand();
    uint8_t gpioStart = analogModule.getGPIOStart();
    DEBUG_PRINTLN("AnalogModuleV1_0 uses " + String(usedGPIOs) + " GPIOs, starting at GPIO " + String(gpioStart));
    TEST_ASSERT_EQUAL(5, usedGPIOs); // Ensure module uses 5 GPIOs

    // Print GPIO numbers
    for (uint8_t i = 0; i < usedGPIOs; i++)
    {
        DEBUG_PRINTLN("Using " + String(CANTramCore::getOutputDefinition(gpioStart + i)->toString()));
    }
}

// Run tests
void setup()
{
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_analogModule_attach);
    RUN_TEST(test_analogModule_initialize);
    RUN_TEST(test_analogModule_interfaces);
    RUN_TEST(measurement_analogModule_loopDuration);
    //RUN_TEST(test_analogModule_output_controlled);
    //RUN_TEST(test_analogModule_input_controlled);
    //RUN_TEST(test_analogModule_input_automatic);
    UNITY_END();
}

void loop()
{
}