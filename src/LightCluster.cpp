//
// Created by emilr on 2022-04-13.
//

#include "LightCluster.h"
#include "main.h"

bool LightCluster::runAnimation() {
    if (shouldRun()) {
        //Serial.println("Animation begin");
        //delay(100);
        animationObject->runAnimation(animationNumber);
        lastRun = millis();

        //Serial.println("Animation call success, setting colors");
        //Serial.println(animationObject->numLeds);
        //delay(100);
        for (int i = 0; i < animationObject->numLeds; i++) {
            /*Serial.print("lights mapped = ");
            Serial.println(animationObject->lights[i].mapped);
            Serial.print("color = ");
            Serial.println(animationObject->lights[i].color, HEX);*/
            leds.setPixelColor(animationObject->lights[i].mapped, animationObject->lights[i].color);
        }
        //Serial.println("Colors set");
        //delay(100);
        //Serial.println("Animation has run");
        return true;
    }
    return false;
}

void LightCluster::runSetup() {
    //Serial.println("Before setup");
    animationObject->runSetup(animationNumber);
    //Serial.println("After setup");
}

void LightCluster::changeAnimation(int newAnimationNumber) {
    animationNumber = newAnimationNumber;
    lastRun = 0;
    runSetup();
    runAnimation();
}

LightCluster::LightCluster(struct light *incomingLights, int size, int animation, Animations *animationObject)
        : animationObject(animationObject) {
//    for (int i = 0; i < sizeof(lights); i++) {
//        light data;
//        data.mapped = lights[i];
//        this->lights[i] = data;
//    }
    //Serial.println("Inside light cluster constructor");
    animationNumber = animation;
    numLights = size;
    animationObject->numLeds = size;
    lastRun = 0;
    lights = incomingLights;
    //Serial.println("About to set up animation object");
    this->animationObject = animationObject;
    //Serial.println("Creating lookup");
    //this->animationObject.createLookup();
    //Serial.println("Construct done, running setup and animation");
    runSetup();
    //Serial.println("Running animation");
    //delay(100);
    runAnimation();
}

LightCluster::~LightCluster() {
    delete[] animationObject->lights;
    delete[] lights;
    delete[] animationObject;
}

bool LightCluster::shouldRun() {
    //Serial.println("ShouldRun started");
    if (millis() - lastRun >= animationObject->delayTimeMS) {
        //Serial.println("ShouldRun true");
        return true;
    }
    //Serial.println("ShouldRun false");
    return false;
}
