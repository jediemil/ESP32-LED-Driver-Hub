//
// Created by emilr on 2022-05-07.
//

#include "Animations.h"

Animations::Animations(light *lights) {
    //Serial.println("Inside animations constructor");
    //delay(100);
    this->lights = lights;
    //Serial.println("Animations constructed");
    //delay(100);
}

/*void Animations::createLookup() {
    Serial.println("Creating tables");
    delay(100);
    //animationPointer[1] = {this->rainbow};
    //setupPointer[1] = {this->setupRainbow};
}*/

void Animations::runAnimation(int animationID) {
    //Serial.println("Sketchy function call");
    //delay(100);
    method_function func = animationPointer[animationID];
    (this->*func)();
    //Serial.println("Animation call success");
    //delay(100);

    //if (animationID == 0) {
    //    this->rainbow();
    //}
}

void Animations::runSetup(int animationID) {
    //Serial.println("Inside setup");
    method_function func = setupPointer[animationID];
    (this->*func)();
    //if (animationID == 0) {
    //    setupRainbow();
    //}
}

void Animations::clearLights() {
    for (int i = 0; i < numLeds; i++) {
        lights[i].animI = 0;
        lights[i].endTick = 0;
        lights[i].state = 0;
        lights[i].color = 0;

        lights[i].targetR = 0;
        lights[i].targetG = 0;
        lights[i].targetB = 0;
        lights[i].targetW = 0;

        lights[i].startR = 0;
        lights[i].startG = 0;
        lights[i].startB = 0;
        lights[i].startW = 0;
    }
}


void Animations::static_color() {
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = animationSetting1;
    }
}

void Animations::setup_static_color() {
    clearLights();
    delayTimeMS = 10000;
    animationSetting1 = 0;
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = animationSetting1;
    }
}


void Animations::rainbow() {
    //Serial.println("Inside animation function");
    //delay(100);
    for (int light = 0; light < numLeds; light++) {
        lights[light].color = leds.ColorHSV(((light * animationSetting1 + animationI * 65536 / maxAnimationI) % 65536),
                                            255, 255);
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

void Animations::setup_rainbow() {
    clearLights();
    animationI = 0;
    delayTimeMS = 100;
    maxAnimationI = 256;
    animationSetting1 = 65536 / (numLeds + 1);
}


void Animations::randomColors() {
    for (int i = 0; i < numLeds; i++) {
        lights[i].animI++;
        /*Serial.println("Inside animation, doing main calc");
        Serial.print("endTick = ");
        Serial.println(lamp->endTick);
        delay(100);*/
        uint32_t r = (lights[i].targetR - lights[i].startR) * lights[i].animI / lights[i].endTick + lights[i].startR;
        uint32_t g = (lights[i].targetG - lights[i].startG) * lights[i].animI / lights[i].endTick + lights[i].startG;
        uint32_t b = (lights[i].targetB - lights[i].startB) * lights[i].animI / lights[i].endTick + lights[i].startB;
        uint32_t w = (lights[i].targetW - lights[i].startW) * lights[i].animI / lights[i].endTick + lights[i].startW;
        //Serial.println("After main calc");

        lights[i].color = w << 24 | r << 16 | g << 8 | b;
        /*Serial.print("R = ");
        Serial.print(r);
        Serial.print(", G = ");
        Serial.print(g);
        Serial.print(", B = ");
        Serial.print(b);
        Serial.print(", W = ");
        Serial.println(w);
        Serial.print("Set color to");
        Serial.println(lamp->color);*/

        if (lights[i].animI == lights[i].endTick) {
            uint8_t minR = (animationSetting1 >> 24) & 0xff;
            uint8_t maxR = (animationSetting2 >> 24) & 0xff;
            uint8_t minG = (animationSetting1 >> 16) & 0xff;
            uint8_t maxG = (animationSetting2 >> 16) & 0xff;
            uint8_t minB = (animationSetting1 >> 8) & 0xff;
            uint8_t maxB = (animationSetting2 >> 8) & 0xff;
            uint8_t minW = animationSetting1 & 0xff;
            uint8_t maxW = animationSetting2 & 0xff;

            lights[i].targetR = random(minR, maxR);
            lights[i].targetG = random(minG, maxG);
            lights[i].targetB = random(minB, maxB);
            lights[i].targetW = random(minW, maxW);

            lights[i].animI = 0;
            lights[i].endTick = random(animationSetting3, animationSetting4);

            lights[i].startR = r;
            lights[i].startG = g;
            lights[i].startB = b;
            lights[i].startW = w;
        }
    }
}

void Animations::setup_randomColors() {
    clearLights();
    delayTimeMS = 100;
    animationSetting1 = 0x00000000;
    animationSetting2 = 0xFFFFFFFF;
    animationSetting3 = 30;
    animationSetting4 = 300;
    for (int i = 0; i < numLeds; i++) {
        //Serial.println("In loop");
        uint8_t minR = (animationSetting1 >> 24) & 0xff;
        uint8_t maxR = (animationSetting2 >> 24) & 0xff;
        uint8_t minG = (animationSetting1 >> 16) & 0xff;
        uint8_t maxG = (animationSetting2 >> 16) & 0xff;
        uint8_t minB = (animationSetting1 >> 8) & 0xff;
        uint8_t maxB = (animationSetting2 >> 8) & 0xff;
        uint8_t minW = animationSetting1 & 0xff;
        uint8_t maxW = animationSetting2 & 0xff;
        //Serial.println("Grabbed max and min");

        lights[i].targetR = random(minR, maxR);
        lights[i].targetG = random(minG, maxG);
        lights[i].targetB = random(minB, maxB);
        lights[i].targetW = random(minW, maxW);

        lights[i].animI = 0;
        lights[i].endTick = random(animationSetting3, animationSetting4);
        //Serial.print("EndTick = ");
        //Serial.println(lamp->endTick);

        lights[i].startR = (lights[i].color >> 16) & 0xff;
        lights[i].startG = (lights[i].color >> 8) & 0xff;
        lights[i].startB = lights[i].color & 0xff;
        lights[i].startW = (lights[i].color >> 24) & 0xff;
        /*Serial.println("New target: ");
        Serial.println(lamp->targetR);
        Serial.println(lamp->targetG);
        Serial.println(lamp->targetB);
        Serial.println(lamp->targetW);
        Serial.println("");*/
    }
}


void Animations::stardust() {
    for (int i = 0; i < numLeds; i++) {
        lights[i].animI++;

        //setColorFromTarget(&lights[i]);

        uint32_t r = (lights[i].targetR - lights[i].startR) * lights[i].animI / lights[i].endTick + lights[i].startR;
        uint32_t g = (lights[i].targetG - lights[i].startG) * lights[i].animI / lights[i].endTick + lights[i].startG;
        uint32_t b = (lights[i].targetB - lights[i].startB) * lights[i].animI / lights[i].endTick + lights[i].startB;
        uint32_t w = (lights[i].targetW - lights[i].startW) * lights[i].animI / lights[i].endTick + lights[i].startW;
        //Serial.println("After main calc");

        lights[i].color = w << 24 | r << 16 | g << 8 | b;

        if (lights[i].animI == lights[i].endTick) {
            if (lights[i].state == 0) { //If light has finished dim up
                lights[i].state = 1; //Dim down

                lights[i].startR = lights[i].targetR;
                lights[i].startG = lights[i].targetG;
                lights[i].startB = lights[i].targetB;
                lights[i].startW = lights[i].targetW;

                lights[i].targetR = 0;
                lights[i].targetG = 0;
                lights[i].targetB = 0;
                lights[i].targetW = 0;

                lights[i].endTick = random(animationSetting3 & 0xffff, (animationSetting3 >> 16) & 0xffff);
                lights[i].animI = 0;
                //Serial.println("Fading down");
            } else if (lights[i].state == 1) {
                lights[i].state = 2; //Wait

                lights[i].startR = 0;
                lights[i].startG = 0;
                lights[i].startB = 0;
                lights[i].startW = 0;

                lights[i].endTick = random(animationSetting1, animationSetting2);
                lights[i].animI = 0;
                //Serial.println("Waiting");
            } else if (lights[i].state == 2) {
                uint32_t newColor = Adafruit_NeoPixel::ColorHSV(random(0, 65535), random(240, 255), random(180, 255));
                lights[i].state = 0; //Dim up

                lights[i].targetR = (newColor >> 16) & 0xff;
                lights[i].targetG = (newColor >> 8) & 0xff;
                lights[i].targetB = newColor & 0xff;
                lights[i].targetW = random(0, 30);

                lights[i].endTick = animationSetting4;
                lights[i].animI = 0;
                /*Serial.println("Set new color, target: ");
                Serial.println(lights[i].targetR);
                Serial.println(lights[i].targetG);
                Serial.println(lights[i].targetB);
                Serial.println(lights[i].targetW);
                Serial.println("Done");*/
            }
        }
    }
    //Serial.println(lights[0].animI);
    //Serial.println(lights[0].color);
}

void Animations::setup_stardust() {
    clearLights();
    delayTimeMS = 20;
    animationSetting1 = 200; //Wait time start
    animationSetting2 = 500; //Wait time end
    animationSetting3 = (30 << 16) + 100; //Fade down time
    animationSetting4 = 5; //Fade up time
    for (int i = 0; i < numLeds; i++) {
        lights[i].state = 2;
        lights[i].endTick = random(animationSetting1, animationSetting2);
    }

    /*Serial.println(animationSetting3);
    Serial.println(animationSetting3 & 0xffff);
    Serial.println((animationSetting3 >> 16) & 0xffff);
    Serial.println(random(animationSetting3 & 0xffff, (animationSetting3 >> 16) & 0xffff));
    Serial.println(lights[0].endTick);
    Serial.println(lights[1].endTick);
    Serial.println(lights[2].endTick);*/
}


void Animations::twinkle() { //Alike stardust but with background color

}

void Animations::setup_twinkle() {
    clearLights();

}


void Animations::ripple() { //B??rja p?? ett slumpm??ssigt st??lle i lights och g?? framm??t eller bak??t med en ljusstrimma

}

void Animations::setup_ripple() {
    clearLights();

}


void Animations::setColorFromTarget(light *lamp) {
    uint32_t r = (lamp->targetR - lamp->startR) * lamp->animI / lamp->endTick + lamp->startR;
    uint32_t g = (lamp->targetG - lamp->startG) * lamp->animI / lamp->endTick + lamp->startG;
    uint32_t b = (lamp->targetB - lamp->startB) * lamp->animI / lamp->endTick + lamp->startB;
    uint32_t w = (lamp->targetW - lamp->startW) * lamp->animI / lamp->endTick + lamp->startW;
    lamp->color = w << 24 | r << 16 | g << 8 | b;
}