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
    long animationSetting1;
    long animationSetting2;
    int numLeds;

    explicit Animations(struct light *lights);
    void runAnimation(int animationId);
    void runSetup(int animationId);

    //void createLookup();

    typedef void (Animations::*method_function)();
    method_function animationPointer[3] = {&Animations::off, &Animations::rainbow, &Animations::randomColors};
    method_function setupPointer[3] = {&Animations::setup_off, &Animations::setup_rainbow, &Animations::setup_randomColors};

    struct light *lights;


private:
    void rainbow();
    void setup_rainbow();

    void off();
    void setup_off();

    void randomColors();
    void setup_randomColors();
};


//#endif //UNTITLED_ANIMATIONS_H
