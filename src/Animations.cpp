//
// Created by emilr on 2022-04-13.
//

#include "Animations.h"
#include "main.h"

int * Animations::runAnimationByNumber(int animation) {

};
int * Animations::setupAnimationByNumber(int animation) {

};


Animations::Animations(int *setupLights) {

}



void Animations::rainbowFill() {
    for (int light = 0; light < numLights; light++) {
        lights[light] = leds.ColorHSV(((light * 360 / 10) + animationI) * 360, 255, 255);
    }
}

void Animations::setupRainbowFill() {
    animationDelayMS = 100;
}