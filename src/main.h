//
// Created by emilr on 2022-04-13.
//
#include <Arduino.h>
#include "../.pio/libdeps/esp32doit-devkit-v1/Adafruit NeoPixel/Adafruit_NeoPixel.h"

#define LED_PIN    4
#define NUM_LEDS 10

#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H

extern Adafruit_NeoPixel leds;

struct light {
    int mapped;
    uint32_t color;

    uint8_t targetR;
    uint8_t targetG;
    uint8_t targetB;
    uint8_t targetW;

    uint8_t startR;
    uint8_t startG;
    uint8_t startB;
    uint8_t startW;

    uint16_t animI;
    uint16_t endTick;
};

#endif //UNTITLED_MAIN_H
