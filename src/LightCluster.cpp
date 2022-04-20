//
// Created by emilr on 2022-04-13.
//

#include "LightCluster.h"
#include "main.h"

int animationI;
int maxAnimationI;
int delayTimeMS;
int animationSetting1;
int animationSetting2;
int numLeds;

extern void rainbow(light lights[]);

extern void setupRainbow();
void (*animationTable[])(light lights[]) = {rainbow};
void (*setup_table[])() = {setupRainbow};

void LightCluster::runAnimation() {
    if (shouldRun()) {
        //Serial.println("Animation begin");
        animationTable[animationNumber](lights);
        lastRun = millis();

        for (int i = 0; i < numLights; i++) {
//            Serial.print("lights mapped = ");
//            Serial.println(lights[i].mapped);
//            Serial.print("color = ");
//            Serial.println(lights[i].color, HEX);
            leds.setPixelColor(lights[i].mapped, lights[i].color);
        }
        //Serial.println("Animation has run");
        //return true;
    }
    //return false;
}

void LightCluster::runSetup() {
    setup_table[animationNumber]();
}

void LightCluster::changeAnimation(int newAnimationNumber) {
    animationNumber = newAnimationNumber;
    runSetup();
    runAnimation();
}

LightCluster::LightCluster(struct light *incomingLights, int size, int animation) {
//    for (int i = 0; i < sizeof(lights); i++) {
//        light data;
//        data.mapped = lights[i];
//        this->lights[i] = data;
//    }
    animationNumber = animation;
    numLights = size;
    numLeds = size;
    lights = incomingLights;
    lastRun = 0;

    runSetup();
    runAnimation();
}

bool LightCluster::shouldRun() {
    //Serial.println("ShouldRun started");
    if (millis() - lastRun >= delayTimeMS) {
        //Serial.println("ShouldRun true");
        return true;
    }
    //Serial.println("ShouldRun false");
    return false;
}

void rainbow(light lights[]) {
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

void setupRainbow() {
    animationI = 0;
    delayTimeMS = 100;
    maxAnimationI = 256;
    animationSetting1 = 65536/(numLeds+1);
}
