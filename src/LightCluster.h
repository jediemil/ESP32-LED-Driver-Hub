//
// Created by emilr on 2022-04-13.
//
#include "stdint.h"
#include "Animations.h"
#include "main.h"

#ifndef UNTITLED_LIGHTCLUSTER_H
#define UNTITLED_LIGHTCLUSTER_H


class LightCluster {
private:
    unsigned long lastRun;

    void runSetup();
    bool shouldRun();

public:
    int numLights;
    int animationNumber;
    uint32_t maxBrightness; //TODO: Make this work
    bool useMaxBrightness;
    uint8_t brightness = 255;

    bool runAnimation(bool force);
    void changeAnimation(int newAnimationNumber);

    LightCluster(light *incomingLights, int size, int animation, Animations *animationObject);
    ~LightCluster();

    light *lights;
    Animations *animationObject;
};


#endif //UNTITLED_LIGHTCLUSTER_H
