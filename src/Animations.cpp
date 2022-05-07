//
// Created by emilr on 2022-05-07.
//

#include "Animations.h"

Animations::Animations(struct light *lights) {
    Serial.println("Inside animations constructor");
    delay(100);
    this->lights = lights;
    Serial.println("Animations constructed");
    delay(100);
}

/*void Animations::createLookup() {
    Serial.println("Creating tables");
    delay(100);
    //animationPointer[1] = {this->rainbow};
    //setupPointer[1] = {this->setupRainbow};
}*/

void Animations::runAnimation(int animationID) {
    /*Serial.println("Sketchy function call");
    delay(100);
    method_function func = animationPointer[animationID];
    (this->*func)();
    Serial.println("Animation call success");
    delay(100);*/

    if (animationID == 0) {
        this->rainbow();
    }
}

void Animations::runSetup(int animationID) {
    Serial.println("Inside setup");
    method_function func = setupPointer[animationID];
    (this->*func)();
    //if (animationID == 0) {
    //    setupRainbow();
    //}
}

void Animations::rainbow() {
    Serial.println("Inside animation function");
    delay(100);
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = leds.ColorHSV(((light * animationSetting1 + animationI*65536/maxAnimationI) % 65536), 255, 255);
//        Serial.print("Set ");
//        Serial.print(light);
//        Serial.print(" to ");
//        Serial.println(leds.ColorHSV(((light * animationSetting1 + animationI) * 360), 255, 255));
//        Serial.println(light * animationSetting1);
//        Serial.println((light * animationSetting1 + animationI) * 360);
    }
    //Serial.print("AnimationI = ");
    //Serial.println(animationI);
    animationI += 1;
    animationI %= maxAnimationI; //Don't go all way to 255 because 0*65536/256 = 0 and 256*65536/256 = 65536. It will result in the same color for 2 cycles.
}

void Animations::setupRainbow() {
    animationI = 0;
    delayTimeMS = 100;
    maxAnimationI = 256;
    animationSetting1 = 65536/(numLeds+1);
}

