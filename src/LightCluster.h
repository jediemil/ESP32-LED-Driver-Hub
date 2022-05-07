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

    void runAnimation();
    void changeAnimation(int newAnimationNumber);

    LightCluster(struct light *incomingLights, int size, int animation, Animations *animationObject);

    Animations *animationObject;
};


#endif //UNTITLED_LIGHTCLUSTER_H
