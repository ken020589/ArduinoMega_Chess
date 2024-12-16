#ifndef LED_H
#define LED_H

#include <Arduino.h>

int pin;
unsigned long previousMillis_led = 0; // 上次改變時間
const long interval_led = 500; // 每 500 毫秒切換一次狀態
bool ledState = LOW; // LED 初始狀態
// 構造函數

// 開啟 LED
void on(int pinNumber) {
    pin = pinNumber;
    pinMode(pin, OUTPUT); // 設置腳位為輸出模式
    digitalWrite(pin, HIGH);
}

// 關閉 LED
void off(int pinNumber) {
    pin = pinNumber;
    pinMode(pin, OUTPUT); // 設置腳位為輸出模式
    digitalWrite(pin, LOW);
}

// 使 LED 閃爍
void blink(int pinNumber) {
    pin = pinNumber;
    pinMode(pin, OUTPUT); // 設置腳位為輸出模式
    unsigned long currentMillis_led = millis(); // 取得目前時間

    // 檢查是否超過間隔時間
    if (currentMillis_led - previousMillis_led >= interval_led) {
        previousMillis_led = currentMillis_led; // 更新上次改變時間

        // 切換 LED 狀態
        ledState = !ledState; 
        digitalWrite(pin, ledState);
    }
}


#endif