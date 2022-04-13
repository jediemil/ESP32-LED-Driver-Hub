//
// Created by emilr on 2022-04-13.
//

#include "LightCluster.h"
#include "Animations.h"

void LightCluster::runAnimation() {

}

void LightCluster::runSetup() {

}

void LightCluster::changeAnimation() {

}

LightCluster::LightCluster(int *lights, int animation) {
    for (int i = 0; i < sizeof(lights); i++) {
        mappedLightTable[i] = lights[i];
    }
    animationNumber = animation;
    numLights = sizeof(lights);

    LocalAnimation = Animations(mappedLightTable);
    runSetup();
}

bool LightCluster::shouldRun() {
    return false;
}
