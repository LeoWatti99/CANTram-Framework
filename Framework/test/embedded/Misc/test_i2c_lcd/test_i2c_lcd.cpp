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
#include "LiquidCrystal_I2C.h"

#define I2C_SDA_PIN 13
#define I2C_SCL_PIN 14

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Runs before tests
void setUp(){

}

//Runs after tests
void tearDown(){
    lcd.clear();
}
void test_i2c_lcd_display(){
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    lcd.begin(16, 2);  // initialize the lcd
    lcd.init(I2C_SDA_PIN, I2C_SCL_PIN);      // initialize the lcd 
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("I2C LCD Test");
    lcd.setCursor(0,1);
    lcd.print("Hello, World!");
    delay(3000); // Wait for 3 seconds to visually confirm the display
    TEST_ASSERT_TRUE(true); // If we reach this point, the test is considered passed
}

/* Write a function for each test*/


//Run tests
void setup(){
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_i2c_lcd_display);
    UNITY_END();
}

void loop(){

}