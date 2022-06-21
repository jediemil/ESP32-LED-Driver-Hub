//
// Created by emilr on 2022-05-07.
//

#include "main.h"

//#ifndef UNTITLED_ANIMATIONS_H
//#define UNTITLED_ANIMATIONS_H

class Animations {
public:
    u_int animationI;
    u_int maxAnimationI;
    u_int delayTimeMS;
    long long animationSetting1;
    long long animationSetting2;
    u_long animationSetting3;
    u_long animationSetting4;
    u_int numLeds;

    explicit Animations(light *lights);
    void runAnimation(int animationId);
    void runSetup(int animationId);

    //void createLookup();

    typedef void (Animations::*method_function)();
    method_function animationPointer[4] = {&Animations::static_color, &Animations::rainbow, &Animations::randomColors, &Animations::stardust};
    method_function setupPointer[4] = {&Animations::setup_static_color, &Animations::setup_rainbow, &Animations::setup_randomColors, &Animations::setup_stardust};

    struct light *lights;


private:
    void setColorFromTarget(light *lamp);

    void rainbow();
    void setup_rainbow();

    void static_color();
    void setup_static_color();

    void randomColors();
    void setup_randomColors();

    void stardust();
    void setup_stardust();

    void twinkle();
    void setup_twinkle();
};


//#endif //UNTITLED_ANIMATIONS_H
