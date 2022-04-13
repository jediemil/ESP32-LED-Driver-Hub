//
// Created by emilr on 2022-04-13.
//

#ifndef UNTITLED_ANIMATIONS_H
#define UNTITLED_ANIMATIONS_H


class Animations {
public:
    int * runAnimationByNumber(int animation);
    int * setupAnimationByNumber(int animation);

    int animationDelayMS;

    Animations(int setupLights[]);
private:
    void rainbowFill();
    void setupRainbowFill();

    int animationI;
    int numLights;
    int lights[];
};


#endif //UNTITLED_ANIMATIONS_H
