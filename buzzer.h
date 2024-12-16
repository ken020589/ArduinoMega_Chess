#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "pitch.h"

// 蜂鳴器連接的腳位
const int buzzerPin = 47;

// 定義音調頻率
const int START_TONE = 423;  // C5
const int MOVE_TONE = 349;   // F4
const int WIN_TONE = 659;    // E5

// 初始化蜂鳴器腳位
void initBuzzer() {
    pinMode(buzzerPin, OUTPUT);
}

// 播放單個音調
void playTone(int frequency, int duration) {
    tone(buzzerPin, frequency);  // 開始播放音調
    delay(duration);             // 持續指定時間
    noTone(buzzerPin);           // 停止音調
}

// 主選單背景音樂
void playMainMenuBGM() {
    int menuMelody[] = {
        NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5
    };
    int menuDurations[] = {
        32, 16, 32, 32, 32, 16
    };
    for (int i = 0; i < sizeof(menuMelody) / sizeof(menuMelody[0]); i++) {
        int duration = 3000 / menuDurations[i];
        tone(buzzerPin, menuMelody[i], duration);
        delay(duration * 1.3);
        noTone(buzzerPin);
    }
}

void playRedBGM() {
    int menuMelody[] = {
        NOTE_E5, NOTE_D5
    };
    int menuDurations[] = {
        32, 32
    };
    for (int i = 0; i < sizeof(menuMelody) / sizeof(menuMelody[0]); i++) {
        int duration = 3000 / menuDurations[i];
        tone(buzzerPin, menuMelody[i], duration);
        delay(duration * 1.3);
        noTone(buzzerPin);
    }
}

void playBlackBGM() {
    int menuMelody[] = {
        NOTE_C5, NOTE_E5
    };
    int menuDurations[] = {
        32, 32
    };
    for (int i = 0; i < sizeof(menuMelody) / sizeof(menuMelody[0]); i++) {
        int duration = 3000 / menuDurations[i];
        tone(buzzerPin, menuMelody[i], duration);
        delay(duration * 1.3);
        noTone(buzzerPin);
    }
}

// 小兵升變音效音符和時長
void playUpgradeBGM() {
    int melody[] = {
        NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6
    };
    int noteDurations[] = {
        32, 32, 32, 16
    };
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
        int duration = 2000 / noteDurations[i];
        tone(buzzerPin, melody[i], duration);
        delay(duration * 1.5);
        noTone(buzzerPin);
    }
}

// 吃子音符和時長
void eatPawnBGM() {
    int melody[] = {
        NOTE_G4, NOTE_C5  // 低音 G4 接高音 C5，形成輕快的兩個音
    };
    int noteDurations[] = {
        32, 32
    };
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
        int duration = 3000 / noteDurations[i];
        tone(buzzerPin, melody[i], duration);
        delay(duration * 1.5);
        noTone(buzzerPin);
    }
}

// 思考音效
void playThinkingBGM() {
    int thinkingMelody[] = {
        NOTE_A3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3
    };
    int thinkingDurations[] = {
        32, 32, 32, 32, 16
    };
    for (int i = 0; i < sizeof(thinkingMelody) / sizeof(thinkingMelody[0]); i++) {
        int duration = 2000 / thinkingDurations[i];
        tone(buzzerPin, thinkingMelody[i], duration);
        delay(duration * 1.5);
        noTone(buzzerPin);
    }
}

// 勝利音效
void playWinBGM() {
    int victoryMelody[] = {
        NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6
    };
    int victoryDurations[] = {
        32, 32, 32, 16
    };
    for (int i = 0; i < sizeof(victoryMelody) / sizeof(victoryMelody[0]); i++) {
        int duration = 3000 / victoryDurations[i];
        tone(buzzerPin, victoryMelody[i], duration);
        delay(duration * 1.3);
        noTone(buzzerPin);
    }
}

#endif