/*
Dieser Test verwendet eine I2C LCD Display (16x2) und einen ESP32.

***
WICHTIG: Vor dem Anschließen müssen die auf dem LCD-Display verbauten
Pullup Widerstände zwischen VCC und SDA sowie VCC und SCL entfernt werden.
***

Verbinden Sie die Pins wie folgt:
ESP32     | I2C LCD
----------------------
GND       | GND
5V       | VCC
GPIO 13   | SDA
GPIO 14   | SCL

WICHTIG: Stellen Sie sicher, dass externe Pull-up-Widerstände (4.7k Ohm) zwischen SDA und VCC sowie SCL und VCC angeschlossen sind.
*/
#include <Arduino.h>
#include <unity.h>
#include "driver/i2c.h"
#include <CANTramCore.h>
#include <ESP32_I2CCore.h>
#include <Debug.h>
#include "LiquidCrystal_I2C.h"

#define I2C_SDA_PIN 13
#define I2C_SCL_PIN 14


#define ADDRESS 0x27

ESP32_I2CCore i2cCore(0);
LiquidCrystal_I2C lcd(ADDRESS, 16, 2); // Set the LCD I2C address


//Runs before tests
void setUp(){
    //init I2C core
}

//Runs after tests
void tearDown(){
    i2cCore.reset();
}


void test_i2c_core_begin(){
    DEBUG_PRINTLN("Testing I2C begin...");
    i2cCore.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    i2cCore.setFrequency(100000); // 100kHz
    i2cCore.setBufferSize(32); // Set buffer size to 32 bytes
    i2cCore.begin();
}

void test_i2c_core_write(){
    DEBUG_PRINTLN("Testing I2C write...");
    
}

void test_i2c_core_lcd(){
    lcd.begin(16,2);
    lcd.init();                      // Initialize the LCD
    lcd.backlight();                 // Turn on the backlight

    lcd.setCursor(0, 0);
    size_t writtenChars = lcd.print("Hello, World!");

    delay(2000);
    lcd.clear();
    TEST_ASSERT_EQUAL(13, writtenChars); // "Hello, World!" has 13 characters
}

/* Write a function for each test*/


//Run tests
void setup(){
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_i2c_core_begin);
    RUN_TEST(test_i2c_core_lcd);
    //RUN_TEST(test_i2c_core_write);
    
    UNITY_END();
}

void loop(){

}