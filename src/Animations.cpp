//
// Created by emilr on 2022-05-07.
//

#include "Animations.h"

Animations::Animations(struct light *lights) {
    //Serial.println("Inside animations constructor");
    //delay(100);
    this->lights = lights;
    //Serial.println("Animations constructed");
    //delay(100);
}

/*void Animations::createLookup() {
    Serial.println("Creating tables");
    delay(100);
    //animationPointer[1] = {this->rainbow};
    //setupPointer[1] = {this->setupRainbow};
}*/

void Animations::runAnimation(int animationID) {
    //Serial.println("Sketchy function call");
    //delay(100);
    method_function func = animationPointer[animationID];
    (this->*func)();
    //Serial.println("Animation call success");
    //delay(100);

    //if (animationID == 0) {
    //    this->rainbow();
    //}
}

void Animations::runSetup(int animationID) {
    //Serial.println("Inside setup");
    method_function func = setupPointer[animationID];
    (this->*func)();
    //if (animationID == 0) {
    //    setupRainbow();
    //}
}


void Animations::off() {
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = 0;
    }
}

void Animations::setup_off() {
    delayTimeMS = 10000;
}


void Animations::rainbow() {
    //Serial.println("Inside animation function");
    //delay(100);
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

void Animations::setup_rainbow() {
    animationI = 0;
    delayTimeMS = 100;
    maxAnimationI = 256;
    animationSetting1 = 65536/(numLeds+1);

}


void Animations::randomColors() {
    for (int i = 0; i < numLeds; i++) {
        light *lamp = &lights[i];

        lamp->animI++;
        /*Serial.println("Inside animation, doing main calc");
        Serial.print("endTick = ");
        Serial.println(lamp->endTick);
        delay(100);*/
        uint32_t r = (lamp->targetR - lamp->startR) * lamp->animI/lamp->endTick + lamp->startR;
        uint32_t g = (lamp->targetG - lamp->startG) * lamp->animI/lamp->endTick + lamp->startG;
        uint32_t b = (lamp->targetB - lamp->startB) * lamp->animI/lamp->endTick + lamp->startB;
        uint32_t w = (lamp->targetW - lamp->startW) * lamp->animI/lamp->endTick + lamp->startW;
        //Serial.println("After main calc");

        lamp->color = w << 24 | r << 16 | g << 8 | b;
        /*Serial.print("R = ");
        Serial.print(r);
        Serial.print(", G = ");
        Serial.print(g);
        Serial.print(", B = ");
        Serial.print(b);
        Serial.print(", W = ");
        Serial.println(w);
        Serial.print("Set color to");
        Serial.println(lamp->color);*/

        if (lamp->animI == lamp->endTick) {
            uint8_t minR = (animationSetting1 >> 24) &0xff;
            uint8_t maxR = (animationSetting2 >> 24) &0xff;
            uint8_t minG = (animationSetting1 >> 16) &0xff;
            uint8_t maxG = (animationSetting2 >> 16) &0xff;
            uint8_t minB = (animationSetting1 >> 8) &0xff;
            uint8_t maxB = (animationSetting2 >> 8) &0xff;
            uint8_t minW = animationSetting1 &0xff;
            uint8_t maxW = animationSetting2 &0xff;

            lamp->targetR = random(minR, maxR);
            lamp->targetG = random(minG, maxG);
            lamp->targetB = random(minB, maxB);
            lamp->targetW = random(minW, maxW);

            lamp->animI = 0;
            lamp->endTick = random(animationSetting3, animationSetting4);

            lamp->startR = r;
            lamp->startG = g;
            lamp->startB = b;
            lamp->startW = w;
        }
    }
}

void Animations::setup_randomColors() {
    delayTimeMS = 100;
    animationSetting1 = 0x00000000;
    animationSetting2 = 0xFFFFFFFF;
    animationSetting3 = 30;
    animationSetting4 = 300;
    for (int i = 0; i < numLeds; i++) {
        //Serial.println("In loop");
        light *lamp = &lights[i];

        uint8_t minR = (animationSetting1 >> 24) &0xff;
        uint8_t maxR = (animationSetting2 >> 24) &0xff;
        uint8_t minG = (animationSetting1 >> 16) &0xff;
        uint8_t maxG = (animationSetting2 >> 16) &0xff;
        uint8_t minB = (animationSetting1 >> 8) &0xff;
        uint8_t maxB = (animationSetting2 >> 8) &0xff;
        uint8_t minW = animationSetting1 &0xff;
        uint8_t maxW = animationSetting2 &0xff;
        //Serial.println("Grabbed max and min");

        lamp->targetR = random(minR, maxR);
        lamp->targetG = random(minG, maxG);
        lamp->targetB = random(minB, maxB);
        lamp->targetW = random(minW, maxW);

        lamp->animI = 0;
        lamp->endTick = random(animationSetting3, animationSetting4); //Between 3 and 30 seconds. TODO add animationSetting3 and 4 for this.
        //Serial.print("EndTick = ");
        //Serial.println(lamp->endTick);

        lamp->startR = (lamp->color >> 16) &0xff;
        lamp->startG = (lamp->color >> 8) &0xff;
        lamp->startB = lamp->color &0xff;
        lamp->startW = (lamp->color >> 24) &0xff;
        /*Serial.println("New target: ");
        Serial.println(lamp->targetR);
        Serial.println(lamp->targetG);
        Serial.println(lamp->targetB);
        Serial.println(lamp->targetW);
        Serial.println("");*/
    }
}
