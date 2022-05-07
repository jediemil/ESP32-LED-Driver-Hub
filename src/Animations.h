//
// Created by emilr on 2022-05-07.
//

#include "main.h"

//#ifndef UNTITLED_ANIMATIONS_H
//#define UNTITLED_ANIMATIONS_H

class Animations {
public:
    int animationI;
    int maxAnimationI;
    int delayTimeMS;
    int animationSetting1;
    int animationSetting2;
    int numLeds;

    explicit Animations(struct light *lights);
    void runAnimation(int animationId);
    void runSetup(int animationId);

    //void createLookup();

    typedef void (Animations::*method_function)();
    method_function animationPointer[1] = {&Animations::rainbow};
    method_function setupPointer[1] = {&Animations::setupRainbow};

    struct light *lights;


private:
    void rainbow();
    void setupRainbow();
};


//#endif //UNTITLED_ANIMATIONS_H
