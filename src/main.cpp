#include "main.h"
#include "LightCluster.h"

#include <WiFi.h>
#include <../.pio/libdeps/esp32doit-devkit-v1/AsyncTCP/src/AsyncTCP.h>
#include "../.pio/libdeps/esp32doit-devkit-v1/ESP Async WebServer/src/ESPAsyncWebServer.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
AsyncWebServer server(80);


Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_WRGB + NEO_KHZ800);

//LightCluster clusters[] = {};
light lampsInCluster1[7] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0}};

LightCluster myCluster(lampsInCluster1, 7, 0);

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void connectSDCard(){
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void setupServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SD, "/index.html", "text/html");
    });

    server.serveStatic("/", SD, "/");

    server.on("/api/create_cluster", HTTP_POST, [](AsyncWebServerRequest *request) {
        String lightString = request->getHeader("lights")->toString();
    });
}

void setup() {
    // write your initialization code here
    Serial.begin(115000);

    connectSDCard();
    connectWiFi();
    setupServer();
    server.begin();

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
    myCluster.runAnimation();
    leds.show();
    delay(1);
}
