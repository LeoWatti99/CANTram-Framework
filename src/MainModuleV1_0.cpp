#include "MainModuleV1_0.h"
#include <Arduino.h>
#include "CANTramCore.h"
#include <SPI.h>
#include "OutputDefinition.h"
#include "Debug.h"
#include "HardwareResource.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"
#include "esp_adc_cal.h"
#include "Adafruit_MCP23X17.h"


const String MainModuleV1_0::HW_TYPE = "MainModuleV1.0";
const String MainModuleV1_0::HW_VERSION = "1.0";
const String MainModuleV1_0::FW_VERSION = "1.0";

/**
 * @brief Reset the module and its hardware cores.
 * @details Calls reset on UART and PWM cores and resets the shift register to return the module to a known default state.
 * @return true if all resets succeeded, false if any reset failed.
 */
bool MainModuleV1_0::reset(){
    INFO_PRINTLN("[MainModuleV1_0] Resetting module...");
    bool result = uart.reset();
    result &= _pwmCore.reset();
    //reset i2c
    result &= resetShiftRegister();

    //reset CAN core
    result &= canCore.reset();

    INFO_PRINTLN("[MainModuleV1_0] Module reset complete.");
    return result;
}

/**
 * @brief Attach this module to a slot and GPIO start index.
 * @details Calls the base CANTramModule attach implementation and performs any module-specific attach logic.
 * @param slot Slot index to attach the module to.
 * @param gpioStart Starting GPIO index assigned to this module.
 * @return true if the attach operation was successful, false otherwise.
 */
bool MainModuleV1_0::attachModule(uint8_t slot, uint8_t gpioStart){
    bool result = true;
    result &= CANTramModule::attachModule(slot, gpioStart);
    
    return result;
}

/**
 * @brief Provide GPIO output definitions to CANTramCore.
 * @details Registers the module's output definition table with CANTramCore so the core can manage GPIO provisioning for this module.
 * @return true if output definitions were added successfully, false otherwise.
 */
bool MainModuleV1_0::provideGPIOs(){

    
    
    //Add output definitions to core
    CANTramCoreError err = CANTramCore::addOutputDefinitions(EXTEND, GPIO_SUPPLY);
    DEBUG_PRINTLN("[MainModuleV1_0] Added " + String(GPIO_SUPPLY) + " output definitions, result: " + String((int)err));
    bool result = (err == CANTramCoreError::CAN_TRAM_OK);

    

    return result;
}

/**
 * @brief Request GPIOs from the core.
 * @details Called by the core when module GPIOs should be configured/allocated. Module should request any runtime GPIOs here.
 * @return true if requests succeeded, false otherwise.
 */
bool MainModuleV1_0::requestGPIOs(){
    return true;
}

/**
 * @brief Provide hardware resources (UART, PWM, etc.) to CANTramCore.
 * @details Attaches hardware resource instances to the core so other modules may request/use them.
 * @return true if all resources were attached successfully, false otherwise.
 */
bool MainModuleV1_0::provideHardwareResources(){
    bool result = true;
    
    result &= CANTramCore::attachHardwareResource(&uart);
    uart.setUARTPort(UART_NUM); 
    result &= CANTramCore::attachHardwareResource(&_pwmCore);
    result &= CANTramCore::attachHardwareResource(&canCore);
    result &= CANTramCore::attachHardwareResource(&i2cCore);
    i2cCore.setMaxFrequency(SHIFTREGISTER_I2C_FREQUENCY); //Set max allowed frequency for I2C bus
    return result;
}

/**
 * @brief Request hardware resources from the core.
 * @details Called when the module should request any hardware resources it needs. Implement module-specific requests here.
 * @return true if all resource requests were granted, false otherwise.
 */
bool MainModuleV1_0::requestHardwareResources(){

    
    return true;
}

/**
 * @brief Register any interfaces provided by this module.
 * @details Add interface instances to the module so the core can enumerate and use them.
 * @return true if interfaces were added successfully, false otherwise.
 */
bool MainModuleV1_0::addInterfaces(){
    
    //Add CAN interface
    canInterface.rename("CAN1");
    canInterface.setCANCore(&canCore);
    _interfaces[0] = &canInterface;

    return true;
}
/**
 * @brief Pre-initialize the module after attachment.
 * @details Initializes I2C core before main intialization, because the shiftregister needs to be started to set the gpios of the extend bus.
 * @return true if pre-initialization succeeded, false otherwise.
 */
bool MainModuleV1_0::preInitialize(){
    bool result=true;
    //Init I2C
    auto initI2C = [&]() -> bool {
        if(i2cCore.getI2CStatus() == I2CCore::I2CStatus::I2C_STARTED) {
            INFO_PRINTLN("[MainModuleV1_0] I2C already started.");
            return true;
        }
        INFO_PRINT("[MainModuleV1_0] Initializing I2C... ");
        i2cCore.setPins(I2C_SDA, I2C_SCL);
        i2cCore.setBufferSize(I2C_BUFFER_LENGTH);
        i2cCore.setI2CNum(I2C_NUM_0);
        
        if(!i2cCore.begin()){
            ERROR_PRINTLN("[MainModuleV1_0] I2C initialization failed! ");
            return false;
        }
        INFO_PRINTLN("Done.");
        return true;
    };
    result &= initI2C();

    auto initShiftRegister = [&]() -> bool {
        INFO_PRINTLN("[MainModuleV1_0] Initializing shift register... ");
        //Wire.begin(I2C_SDA, I2C_SCL,I2CCore::I2C_DEFAULT_FREQUENCY); //should already be initialized by i2cCore
        bool res = startShiftRegister();
        return res;
    };
    result &= initShiftRegister();

    auto initOutputs = [&]() -> bool {
    //Configure outputs
        bool b=true;
        INFO_PRINTLN("[MainModuleV1_0] Configuring outputs...");
        for (OutputDefinition& def : EXTEND) {
         b &= configureOutput(&def);
        }
        return b;
    };
    result &= initOutputs();
    
    return result;
}
/**
 * @brief Initialize the module and its hardware subsystems.
 * @details Initializes UART, PWM, I2C, SPI and ADC subsystems as required by the module. This prepares the module for operation after attachment and resource provisioning.
 * @return true if all initialization steps succeeded, false if any step failed.
 */
bool MainModuleV1_0::initialize(){
    bool result = true;

    

    /**
     * @brief Initialize the shift register for GPIO expansion.
     * @details Configuration of shift register must happen before initialization of the core!
     * @details Otherwise errors will be generated since the core is not able to set the initial values of these outputs
     * @return true if initialization succeeded, false otherwise.
     */
    

    

    //Init UART
    auto initUART = [&]() -> bool {
        if(!uart.isUsed()) return true;
        INFO_PRINT("[MainModuleV1_0] Initializing UART... ");
        if(!uart.install(UART_NUM, UART_TX, UART_RX, UART_BUFFER_SIZE)){
            ERROR_PRINTLN("[MainModuleV1_0] UART initialization failed! Programm stopped.");
            return false;
        }
        if(!uart.applyConfig()){
            ERROR_PRINTLN("[MainModuleV1_0] UART configuration failed! Programm stopped.");
            return false;
        }
        if(!uart.applyPins()){
            ERROR_PRINTLN("[MainModuleV1_0] UART pin assignment failed! Programm stopped.");
            return false;
        }
        INFO_PRINTLN("Done.");
        return true;
    };
    result &= initUART();

    //Init PWM
    auto initPWM = [&]() -> bool {
        if(!_pwmCore.isUsed()) return true;
        INFO_PRINT("[MainModuleV1_0] Initializing PWM... ");
        _pwmCore.begin();
        
        INFO_PRINTLN("Done.");
        return true;
    };
    result &= initPWM();

    

    //Init SPI
    auto initSPI = [&]() -> bool {
        INFO_PRINT("[MainModuleV1_0] Initializing SPI... ");
        SPI.begin(SPI_SCK, SPI_CIPO, SPI_COPI, -1); // -1 for CS pin, as we will set it manually
        if (!SPI.bus()) {
            CRITICAL_ERROR_PRINTLN("[MainModuleV1_0] SPI initialization failed! No SPI bus available. Program stopped.");
            while(1);
        };
        INFO_PRINTLN("Done.");
        return true;
    };
    result &= initSPI();

    //Init ADC for temperature measurement
    auto initADC = [&]() -> bool {
        INFO_PRINTLN("[MainModuleV1_0] Initializing ADC for temperature measurement... ");

        #ifdef CONFIG_IDF_TARGET_ESP32_C3
        //Init DMA Controller for ADC
        adc_digi_init_config_t adc_init_config = {
            .max_store_buf_size = TEMP_MULTISAMPLE * sizeof(adc_digi_output_data_t),
            .conv_num_each_intr = TEMP_MULTISAMPLE,
            .adc1_chan_mask = (1 << TEMP_ADC_CHANNEL),
            .adc2_chan_mask = 0
        };

        //Configuration for used ADC Channel
        adc_digi_pattern_config_t adc_pattern = {
            .atten = ADC_ATTEN_DB_12,
            .channel = TEMP_ADC_CHANNEL,
            .unit = ADC_UNIT_1,
            .bit_width = ADC_WIDTH_BIT_11,
            
            
        };

        //Configuration for digital controller
        adc_digi_configuration_t adc_config = {
            .conv_limit_en = false,
            .conv_limit_num = 0,
            .pattern_num = 1,
            .adc_pattern = &adc_pattern,
            .sample_freq_hz = 1024,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1 //uses 11bit for data
        };

        adc_digi_initialize(&adc_init_config);
        adc_digi_controller_configure(&adc_config);
        adc_digi_start();
        INFO_PRINTLN("[MainModuleV1_0] ADC for temperature measurement initialized. Multisampling: " + String(TEMP_MULTISAMPLE) + " samples.");

        #elif CONFIG_IDF_TARGET_ESP32

        INFO_PRINTLN("[MainModuleV1_0] Calibrating ADC... ");
        //Calibrate ADC
        esp_err_t ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP);
        if(!(ret == ESP_OK)) {
            WARNING_PRINTLN("[MainModuleV1_0] Warning: eFuse Two Point values not supported. ADC calibration disabled.");
        }else{
            INFO_PRINTLN("[MainModuleV1_0] eFuse Two Point values supported. ADC calibration enabled.");
            esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, ADC_DEFAULT_VREF, &adc1_chars);
            adc_calibrated = true;
        }

        ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
        if(!(ret == ESP_OK)) {
            WARNING_PRINTLN("[MainModuleV1_0] Warning: eFuse Vref value not supported. ADC calibration disabled.");
        }else{
            INFO_PRINTLN("[MainModuleV1_0] eFuse Vref value supported. ADC calibration enabled.");
            esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, ADC_DEFAULT_VREF, &adc1_chars);
            adc_calibrated = true;
        }
        

        //Configure single read mode for temperature measurement
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(TEMP_ADC_CHANNEL, ADC_ATTEN_DB_12);
        INFO_PRINTLN("[MainModuleV1_0] ADC for temperature measurement initialized.(Hardware multisampling not supported)");

        #endif

        

        return true;
    };
    result &= initADC();

    

    

    //Init CAN
    auto initCAN = [&]() -> bool {
        INFO_PRINT("[MainModuleV1_0] Initializing CAN... ");

        //set pins for can 
        canCore.setPins(CAN_TX, CAN_RX);
        if(canCore.getBaudrate() == CANCore::BR_NOT_SET){
            //Set default baudrate if not set
            canCore.setBaudrate(CANTramCore::DEFAULT_CAN_BAUDRATE);
        }
        if(!canCore.begin()){
            ERROR_PRINTLN("[MainModuleV1_0] CAN initialization failed! Program stopped.");
            canInterface.invalidate();
            return false;
        }

        //Baudrate should be set by CAN interface when requested by user, so just set default here
        
        INFO_PRINTLN("Done.");
        return true;
    };
    result &= initCAN();

    //Init LEDs
    shiftRegister.pinMode(LED_WARNING, OUTPUT);
    shiftRegister.pinMode(LED_ERROR, OUTPUT);

    return result;
}

/**
 * @brief Periodic cycle callback for the module.
 * @details Called by the core to perform periodic tasks. Implement module-specific cyclic behavior here.
 * @param response Pointer to a response buffer where the module may write response bytes. May be nullptr if not used.
 */
void MainModuleV1_0::cycle(uint8_t* response){

    getTemperatureCelsius(); //Just read temperature periodically

    //Not implemented correctly yet, just for testing
    #warning "DEV WARNING: LED handling in MainModuleV1_0::cycle() is just for testing purposes!. Remove or implement proper status LED handling!"
    shiftRegister.digitalWrite(LED_WARNING, 1); //Toggle warning LED
    shiftRegister.digitalWrite(LED_ERROR, 1); //Toggle error
}



/**
 * @brief Set PWM duty for an output definition.
 * @details Validates the output definition for PWM capability and delegates to the PWM core to set the duty cycle.
 * @param def Pointer to the OutputDefinition describing the output.
 * @param value Duty value to set (implementation-specific range).
 * @return true if PWM was set successfully, false on error (e.g., non-PWM output).
 */
bool MainModuleV1_0::setPWM(OutputDefinition* def, uint16_t value) {
    if(!def->pwm) {
        ERROR_PRINTLN("[MainModuleV1_0] ERROR: Attempted to set PWM on non-PWM output "+ String(def->toString()));
        return false;
    }
    if(def->isShift) {
        ERROR_PRINTLN("[MainModuleV1_0] ERROR: Shift register PWM output not implemented yet for output "+ String(def->toString()));
        return false;
    } else {
        // Handle direct GPIO PWM output
        _pwmCore.setDuty(def, value);
        INFO_PRINTLN("[MainModuleV1_0] INFO: Setting PWM on GPIO pin " + String(def->pinOrBit) + " to " + String(value));
        return true; //Assume always successful for now
    }
}

/**
 * @brief Register a PWM channel for an output.
 * @details Initializes PWM core and requests a channel allocation for the given output definition.
 * @param def Pointer to the OutputDefinition to register for PWM.
 * @return true if channel registration succeeded, false otherwise.
 */
bool MainModuleV1_0::registerPWMChannel(OutputDefinition* def) {
    _pwmCore.begin();
    if(!def->pwm) {
        ERROR_PRINTLN("[MainModuleV1_0] Attempted to register PWM channel on non-PWM output "+ String(def->toString()));
        return false;
    }
    if(def->isShift) {
        ERROR_PRINTLN("[MainModuleV1_0] Attempted to register PWM channel on shift register output "+ String(def->toString()));
        return false;
    } else {
        return _pwmCore.addChannel(def);
    }
}
/**
 * @brief Get the temperature in Celsius in 0.1°C steps
 * @details Reads ADC samples (digital controller on C3, single reads on ESP32) and converts them to temperature using calibration or approximation. Returns temperature in tenths of a degree Celsius (e.g., 253 means 25.3°C).
 * @return int16_t Temperature in 0.1°C steps.
 */
uint16_t MainModuleV1_0::getTemperatureCelsius() {

    #ifdef CONFIG_IDF_TARGET_ESP32_C3
    //Using digital controller with DMA
   //Read multiple samples and calculate average
    uint8_t adc_reading[TEMP_MULTISAMPLE*sizeof(adc_digi_output_data_t)];
    uint32_t total = 0;
    adc_digi_read_bytes((uint8_t*)adc_reading, sizeof(adc_reading),&total, ADC_MAX_DELAY);

    //calculate sum of samples
    adc_digi_output_data_t* data = (adc_digi_output_data_t*)adc_reading;  //Pointer to the data
    uint32_t sum = 0;
    for(int i=0; i < TEMP_MULTISAMPLE; i++) {
        sum += data[i].type1.data; //Using type1 as we configured ADC to use 12bit format
    }
    DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature reading sum: " + String(sum));
    DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature samples: "+String(total));

    //calculate average
    uint16_t avg = sum / TEMP_MULTISAMPLE;
    DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature reading average: " + String(avg));
    #elif CONFIG_IDF_TARGET_ESP32

    //Using single read mode with hardware multisampling not supported
    uint32_t avg;
    uint16_t voltage;
    avg = adc1_get_raw(TEMP_ADC_CHANNEL);
    DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature reading raw value: " + String(avg));

    if(adc_calibrated) {
        //Convert raw reading to voltage in millivolts using calibration
        voltage = esp_adc_cal_raw_to_voltage(avg, &adc1_chars);
        DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature reading voltage(calibrated): " + String(voltage) + " mV");
    }else{
        //Convert to voltage in millivolts
        voltage = uint16_t(avg * ADC_MAX_VOLTAGE / 4095); //12bit ADC max value is 4095
        DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature reading voltage(uncalibrated): " + String(voltage) + " mV");

    }
    #endif

    //Convert to sensor voltage in millivolts
    uint16_t sensorVoltage = uint16_t((float)voltage / TEMP_FAKTOR);
    DEBUG_PRINTLN("[MainModuleV1_0] ADC Temperature sensor voltage: " + String(sensorVoltage) + " mV");

    //Convert to temperature using the sensor characteristics
    int16_t temperatureC = float(sensorVoltage - 1375) / float(22.5f)*10.0f; //in 0.1°C steps
    DEBUG_PRINTLN("[MainModuleV1_0] Calculated temperature: " + String(temperatureC/10.0f) + " °C");
    
    return temperatureC;
}





