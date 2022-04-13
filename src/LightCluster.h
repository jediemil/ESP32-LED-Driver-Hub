//
// Created by emilr on 2022-04-13.
//

#include "Animations.h"

#ifndef UNTITLED_LIGHTCLUSTER_H
#define UNTITLED_LIGHTCLUSTER_H


class LightCluster {
public:
    int numLights;
    int animationNumber;

    void runAnimation();
    void changeAnimation();
    bool shouldRun();

    LightCluster(int *lights, int animation);



private:
    long lastRun;
    Animations LocalAnimation;

    void runSetup();

    int mappedLightTable[];
};


#endif //UNTITLED_LIGHTCLUSTER_H
