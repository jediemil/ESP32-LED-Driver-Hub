#include "main.h"
#include "LightCluster.h"

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_WRGB + NEO_KHZ800);

//LightCluster clusters[] = {};
light lampsInCluster1[7] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0}};

LightCluster myCluster(lampsInCluster1, 7, 0);

void setup() {
    // write your initialization code here
    Serial.begin(115000);
    delay(1000);
    leds.begin();
    leds.clear();

    Serial.println("Started");

    //int lampsInCluster1[] = {0,1,2,3,4,5,6,7,8,9};
    //clusters[0] = LightCluster(lampsInCluster1, 0);
    Serial.println("Cluster made");

}

void loop() {
    /*leds.clear();
    delay(1000);
    for (int i = 0; i < 360; i++) {
        for (int light = 0; light < NUM_LEDS; light++) {
            leds.setPixelColor(light, leds.ColorHSV(((light * 360 / 10) + i) * 360, 255, 255));
        }
        leds.show();
        delay(200);
    }
    leds.clear();
    delay(1000);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds.setPixelColor(i, 0, 255, 0, 0);
        leds.show();
        delay(1000);
    }

    leds.clear();
    leds.show();
    delay(1000);

    for (int i = 0; i < 10; i++) {
        leds.clear();
        for (int light = 0; light < NUM_LEDS; light++) {
            if (random(2) == 1) {
                leds.setPixelColor(light, rand());
            }
        }
        leds.show();
        delay(1000);
    }*/
//    Serial.println("In loop");
//    delay(1000);
//    Serial.println("In loop2");
    myCluster.runAnimation();
    //if (hasRun) {
      //  Serial.println("Running show");
      //  leds.show();
   // }
   leds.show();
   delay(1);
}
