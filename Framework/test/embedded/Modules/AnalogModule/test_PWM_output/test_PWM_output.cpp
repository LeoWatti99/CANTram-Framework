#include <Arduino.h>
#include <unity.h>
#include <driver/ledc.h>
#include "Debug.h"

#define LEDC_TEST_CHANNEL LEDC_CHANNEL_0
#define LEDC_TEST_GPIO    15
#define LEDC_TEST_TIMER  LEDC_TIMER_1
#define LEDC_TEST_SPEED_MODE LEDC_HIGH_SPEED_MODE


//Runs before tests
void setUp(){

}

//Runs after tests
void tearDown(){

}
void test_timer_config(){
    DEBUG_PRINTLN("Starting test_timer_config");
    ledc_timer_config_t testTimerConfig{
        .speed_mode       = LEDC_TEST_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_12_BIT, // Set duty resolution to 12 bits
        .timer_num        = LEDC_TEST_TIMER,     // Use timer 1
        .freq_hz          = 5000,              // Set frequency to 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK      // Auto select the source clock
        
    };

    esp_err_t result = ledc_timer_config(&testTimerConfig);
    TEST_ASSERT_EQUAL(ESP_OK, result);
}

void test_channel_config(){
    DEBUG_PRINTLN("Starting test_channel_config");
    ledc_channel_config_t testChannelConfig;
    testChannelConfig.channel    = LEDC_TEST_CHANNEL;
    testChannelConfig.duty       = 0; // Set duty to 0%
    testChannelConfig.gpio_num   = LEDC_TEST_GPIO;
    testChannelConfig.speed_mode = LEDC_TEST_SPEED_MODE;
    testChannelConfig.hpoint     = 0;
    testChannelConfig.timer_sel  = LEDC_TEST_TIMER;
    testChannelConfig.intr_type  = LEDC_INTR_DISABLE; // Add this line
#if ESP_IDF_VERSION_MAJOR >= 4
    testChannelConfig.flags.output_invert = 0; // Add this line for ESP-IDF v4+
#endif

    esp_err_t result = ledc_channel_config(&testChannelConfig);
    TEST_ASSERT_EQUAL(ESP_OK, result);
}

void test_set_duty_cycle(){
    DEBUG_PRINTLN("Starting test_set_duty_cycle");
    uint32_t duty = (1 << 12) / 2; // 50% duty cycle for 12-bit resolution
    esp_err_t result = ledc_set_duty(LEDC_TEST_SPEED_MODE, LEDC_TEST_CHANNEL, duty);
    TEST_ASSERT_EQUAL(ESP_OK, result);

    result = ledc_update_duty(LEDC_TEST_SPEED_MODE, LEDC_TEST_CHANNEL);
    TEST_ASSERT_EQUAL(ESP_OK, result);
}


//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_timer_config);
    RUN_TEST(test_channel_config);
    RUN_TEST(test_set_duty_cycle);
    UNITY_END();
}

void loop(){

}