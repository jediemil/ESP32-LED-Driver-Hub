//
// Created by emilr on 2022-05-07.
//

#include "Animations.h"

Animations::Animations(light *lights) {
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


void Animations::static_color() {
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = animationSetting1;
    }
}

void Animations::setup_static_color() {
    delayTimeMS = 10000;
    animationSetting1 = 0;
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = animationSetting1;
    }
}


void Animations::rainbow() {
    //Serial.println("Inside animation function");
    //delay(100);
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = leds.ColorHSV(((light * animationSetting1 + animationI * 65536 / maxAnimationI) % 65536),
                                            255, 255);
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
    animationSetting1 = 65536 / (numLeds + 1);

}


void Animations::randomColors() {
    for (int i = 0; i < numLeds; i++) {
        lights[i].animI++;
        /*Serial.println("Inside animation, doing main calc");
        Serial.print("endTick = ");
        Serial.println(lamp->endTick);
        delay(100);*/
        uint32_t r = (lights[i].targetR - lights[i].startR) * lights[i].animI / lights[i].endTick + lights[i].startR;
        uint32_t g = (lights[i].targetG - lights[i].startG) * lights[i].animI / lights[i].endTick + lights[i].startG;
        uint32_t b = (lights[i].targetB - lights[i].startB) * lights[i].animI / lights[i].endTick + lights[i].startB;
        uint32_t w = (lights[i].targetW - lights[i].startW) * lights[i].animI / lights[i].endTick + lights[i].startW;
        //Serial.println("After main calc");

        lights[i].color = w << 24 | r << 16 | g << 8 | b;
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

        if (lights[i].animI == lights[i].endTick) {
            uint8_t minR = (animationSetting1 >> 24) & 0xff;
            uint8_t maxR = (animationSetting2 >> 24) & 0xff;
            uint8_t minG = (animationSetting1 >> 16) & 0xff;
            uint8_t maxG = (animationSetting2 >> 16) & 0xff;
            uint8_t minB = (animationSetting1 >> 8) & 0xff;
            uint8_t maxB = (animationSetting2 >> 8) & 0xff;
            uint8_t minW = animationSetting1 & 0xff;
            uint8_t maxW = animationSetting2 & 0xff;

            lights[i].targetR = random(minR, maxR);
            lights[i].targetG = random(minG, maxG);
            lights[i].targetB = random(minB, maxB);
            lights[i].targetW = random(minW, maxW);

            lights[i].animI = 0;
            lights[i].endTick = random(animationSetting3, animationSetting4);

            lights[i].startR = r;
            lights[i].startG = g;
            lights[i].startB = b;
            lights[i].startW = w;
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
        uint8_t minR = (animationSetting1 >> 24) & 0xff;
        uint8_t maxR = (animationSetting2 >> 24) & 0xff;
        uint8_t minG = (animationSetting1 >> 16) & 0xff;
        uint8_t maxG = (animationSetting2 >> 16) & 0xff;
        uint8_t minB = (animationSetting1 >> 8) & 0xff;
        uint8_t maxB = (animationSetting2 >> 8) & 0xff;
        uint8_t minW = animationSetting1 & 0xff;
        uint8_t maxW = animationSetting2 & 0xff;
        //Serial.println("Grabbed max and min");

        lights[i].targetR = random(minR, maxR);
        lights[i].targetG = random(minG, maxG);
        lights[i].targetB = random(minB, maxB);
        lights[i].targetW = random(minW, maxW);

        lights[i].animI = 0;
        lights[i].endTick = random(animationSetting3, animationSetting4);
        //Serial.print("EndTick = ");
        //Serial.println(lamp->endTick);

        lights[i].startR = (lights[i].color >> 16) & 0xff;
        lights[i].startG = (lights[i].color >> 8) & 0xff;
        lights[i].startB = lights[i].color & 0xff;
        lights[i].startW = (lights[i].color >> 24) & 0xff;
        /*Serial.println("New target: ");
        Serial.println(lamp->targetR);
        Serial.println(lamp->targetG);
        Serial.println(lamp->targetB);
        Serial.println(lamp->targetW);
        Serial.println("");*/
    }
}
