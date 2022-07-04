#include "main.h"
#include "LightCluster.h"
#include "WiFiPass.h"
//#include "updater.cpp"

#include <WiFi.h>
#include <../.pio/libdeps/esp32doit-devkit-v1/AsyncTCP/src/AsyncTCP.h>
#include "../.pio/libdeps/esp32doit-devkit-v1/ESP Async WebServer/src/ESPAsyncWebServer.h"
#include "../.pio/libdeps/esp32doit-devkit-v1/ESP Async WebServer/src/AsyncJson.h"
#include "../.pio/libdeps/esp32doit-devkit-v1/ArduinoJson/src/ArduinoJson/Deserialization/DeserializationError.hpp"
#include <SPI.h>
#include <SD.h>
#include <../.pio/libdeps/esp32doit-devkit-v1/IRremote/src/IRremote.hpp>
#include <../.pio/libdeps/esp32doit-devkit-v1/ArduinoJson/src/ArduinoJson.h>

#include <Update.h>
#define U_PART U_SPIFFS
size_t content_len;

String sdCursor = "/";

#define IR_RECEIVE_PIN 15

uint32_t lastIrReceive = 0;

AsyncWebServer server(80);

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_WRGB + NEO_KHZ800);

LightCluster *clusters[10];
int numClusters = 0;

int occupiedLamps[NUM_LEDS];
int numOccupiedLamps = 0;

bool extShow = false;

//LightCluster myCluster(lampsInCluster1, 7, 0, Animations(lampsInCluster1));

void updateOccupiedLamps() {
    for (int & occupiedLamp : occupiedLamps) {
        occupiedLamp = -1;
    }

    int numFoundLamps = 0;
    for (int i = 0; i < numClusters; i++) {
        for (int lamp = 0; lamp < clusters[i]->numLights; lamp++) {
            numFoundLamps++;
            occupiedLamps[clusters[i]->animationObject->lights[lamp].mapped] = i;
        }
    }
    numOccupiedLamps = numFoundLamps;
}

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

void connectSDCard() {
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        ESP.restart();
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE){
        Serial.println("No SD card attached");
        ESP.restart();
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC){
        Serial.println("MMC");
    } else if (cardType == CARD_SD){
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
        ESP.restart();
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index){
        Serial.println("Update");
        content_len = request->contentLength();
        // if filename includes spiffs, update the spiffs partition
        int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len) {
        Update.printError(Serial);
    }

    if (final) {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
        response->addHeader("Refresh", "10");
        response->addHeader("Location", "/");
        request->send(response);
        if (!Update.end(true)){
            Update.printError(Serial);
        } else {
            Serial.println("Update complete");
            Serial.flush();
            ESP.restart();
        }
    }
}

void printProgress(size_t prg, size_t sz) {
    Serial.printf("Progress: %d%%\n", (prg*100)/content_len);
}


void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
        logmessage = "Upload Start: " + String(filename);

        String storeLocation = sdCursor + "/" + filename;

        if (filename.endsWith(".js")) {
            storeLocation = "/js/" + filename;
        } else if (filename.endsWith(".css")) {
            storeLocation = "/css/" + filename;
        } else if (filename.endsWith(".html")) {
            storeLocation = "/" + filename;
        }

        // open the file on first call and store the file handle in the request object
        if (SD.exists(storeLocation)) {
            SD.remove(storeLocation);
        }
        request->_tempFile = SD.open(storeLocation, "w");
        Serial.println(logmessage);
    }

    if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
        logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
        Serial.println(logmessage);
    }

    if (final) {
        logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
        // close the file handle as the upload is now done
        request->_tempFile.close();
        Serial.println(logmessage);
        request->redirect("/");
    }
}

String humanReadableSize(const size_t bytes) {
    if (bytes < 1024) return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
    else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
    else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

String listFiles(bool ishtml) {
    String returnText = "";
    Serial.println("Listing files stored on SD");
    File root = SD.open(sdCursor);
    //Serial.println(root.name());
    File foundfile = root.openNextFile();
    //Serial.println(foundfile.name());
    if (ishtml) {
        returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
    }
    while (foundfile) {
        Serial.println(foundfile.name());
        if (ishtml) {
            if (foundfile.isDirectory()) {
                returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
                returnText += "<td><button onclick=\"openDirectory(\'" + String(foundfile.name()) + "\')\">Open directory</button></tr>"; //<button onclick=\"deleteFile(\'" + String(foundfile.name()) + "\')\">Delete directory</button></tr>";
            } else {
                returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
                returnText += "<td><button onclick=\"deleteFile(\'" + String(foundfile.name()) + "\')\">Delete</button></tr>";
            }
        } else {
            returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
        }
        foundfile.close();
        foundfile = root.openNextFile();
    }
    if (ishtml) {
        returnText += "</table>";
    }
    foundfile.close();
    root.close();
    return returnText;
}


void setupServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SD, "/index.html", "text/html");
    });

    server.serveStatic("/", SD, "/");

    server.on("/api/get_config", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);
        json["numClusters"] = numClusters;
        json["clusters"] = DynamicJsonDocument(1024);

        for (int cluster = 0; cluster < numClusters; cluster++) { //    TODO: Send settings too
            json["clusters"][cluster]["numLights"] = clusters[cluster]->numLights;
            json["clusters"][cluster]["animation"] = clusters[cluster]->animationNumber;

            json["clusters"][cluster]["animationSetting1"] = clusters[cluster]->animationObject->animationSetting1;
            json["clusters"][cluster]["animationSetting2"] = clusters[cluster]->animationObject->animationSetting2;
            json["clusters"][cluster]["animationSetting3"] = clusters[cluster]->animationObject->animationSetting3;
            json["clusters"][cluster]["animationSetting4"] = clusters[cluster]->animationObject->animationSetting4;
            json["clusters"][cluster]["delayTime"] = clusters[cluster]->animationObject->delayTimeMS;

            json["clusters"][cluster]["lights"] = DynamicJsonDocument(1024);
            for (int lamp = 0; lamp < clusters[cluster]->numLights; lamp++) {
                json["clusters"][cluster]["lights"][lamp] = clusters[cluster]->lights[lamp].mapped;
            }
        }
        serializeJson(json, *response);
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        request->send(response);
    });


    server.on("/api/set_config", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<400> doc;
        DeserializationError error = deserializeJson(doc, request->header("data"));
        Serial.print(request->header("data"));
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        for (int i = 0; i < numClusters; i++) {
            delete[]clusters[i];
        }

        int numNewClusters = doc["numClusters"];
        for (int i = 0; i < numNewClusters; i++) {
            int newNumLights = doc["clusters"][i]["numLights"];
            auto newLights = new light[newNumLights];

            for (int lamp = 0; lamp < newNumLights; lamp++) {
                int mappedLight = doc["clusters"][i]["lights"][lamp];
                auto lampa = new light();
                lampa->mapped = mappedLight;
                lampa->color = 0;
                newLights[lamp] = *lampa;
            }

            auto newAnimationObject = new Animations(newLights);
            auto newLightCluster = new LightCluster(newLights, newNumLights, doc["clusters"][i]["animation"], newAnimationObject);
            clusters[i] = newLightCluster;
            numClusters = numNewClusters;
            extShow = true;

            updateOccupiedLamps();

            AsyncResponseStream *response = request->beginResponseStream("text/plain");
            response->addHeader("Access-Control-Allow-Origin","*");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
            response->addHeader("Access-Control-Allow-Methods", "POST, GET");
            response->setCode(200);
            request->send(response);
        }
    });

    server.on("/api/change_setting", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, request->getHeader("data")->value().c_str());
        Serial.println(request->getHeader("data")->value().c_str());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        int cluster = doc["targetCluster"];
        if (cluster >= 0) {
            long long value = doc["newValue"];
            if (doc["setting"] == "delayTime") {
                clusters[cluster]->animationObject->delayTimeMS = value;
                Serial.println(clusters[cluster]->animationObject->delayTimeMS);
            } else if (doc["setting"] == "animationSetting1") {
                clusters[cluster]->animationObject->animationSetting1 = value;
            } else if (doc["setting"] == "animationSetting2") {
                clusters[cluster]->animationObject->animationSetting2 = value;
            } else if (doc["setting"] == "animationSetting3") {
                clusters[cluster]->animationObject->animationSetting3 = value;
            } else if (doc["setting"] == "animationSetting4") {
                clusters[cluster]->animationObject->animationSetting4 = value;
            } else if (doc["setting"] == "maxAnimationI") {
                clusters[cluster]->animationObject->maxAnimationI = value;
            }
            extShow = true;
            Serial.println(value);
        }

        Serial.println(cluster);

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);
    });

    server.on("/api/change_animation", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, request->getHeader("data")->value().c_str());
        Serial.println(request->getHeader("data")->value().c_str());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        int cluster = doc["targetCluster"];
        int value = doc["animation"];
        clusters[cluster]->changeAnimation(value);
        extShow = true;

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);
    });

    server.on("/api/create_cluster", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, request->getHeader("data")->value().c_str());
        Serial.println(request->getHeader("data")->value().c_str());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        int numLights = doc["numLights"];

        bool containsOccupiedLamp = false;
        for (int i = 0; i < numLights; i++) {
            int mappedLight = doc["lights"][i];
            containsOccupiedLamp = containsOccupiedLamp || occupiedLamps[mappedLight] >= 0;
        }

        Serial.println(containsOccupiedLamp);
        for (int i = 0; i < NUM_LEDS; i++) {
            Serial.print("Lamp ");
            Serial.print(i);
            Serial.print(" -> ");
            Serial.println(occupiedLamps[i]);
        }

        if (!containsOccupiedLamp) {
            auto newLights = new light[numLights];

            for (int i = 0; i < numLights; i++) {
                int mappedLight = doc["lights"][i];
                auto lamp = new light();
                lamp->mapped = mappedLight;
                lamp->color = 0;
                newLights[i] = *lamp;
            }

            auto newAnimationObject = new Animations(newLights);
            auto newLightCluster = new LightCluster(newLights, numLights, 0, newAnimationObject);
            clusters[numClusters] = newLightCluster;
            numClusters++;
            extShow = true;
        }

        updateOccupiedLamps();

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);
    });

    server.on("/api/delete_cluster", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, request->getHeader("data")->value().c_str());
        Serial.println(request->getHeader("data")->value().c_str());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }


        int targetCluster = doc["cluster"];
        for (int i = 0; i < numClusters; i++) {
            if (i == targetCluster) {
                for (int lamp = 0; lamp < clusters[i]->numLights; lamp++) {
                    leds.setPixelColor(clusters[i]->animationObject->lights[lamp].mapped, 0);
                }
                delete clusters[i];
            }
            if (i > targetCluster) {
                clusters[i-1] = clusters[i];
            }
        }
        extShow = true;
        numClusters--;

        updateOccupiedLamps();

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);
    });

    server.on("/api/sync", HTTP_POST, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, request->getHeader("data")->value().c_str());
        Serial.println(request->getHeader("data")->value().c_str());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        for (int i = 0; i < numClusters; i++) {
            clusters[i]->animationObject->animationI = 0;
        }

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);
    });

    server.on("/admin/doUpdate", HTTP_POST,
              [](AsyncWebServerRequest *request) {},
              [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                 size_t len, bool final) {
        handleDoUpdate(request, filename, index, data, len, final);
    });
    Update.onProgress(printProgress);

    server.on("/admin/upload_file", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
    }, handleUpload);

    server.on("/admin/listfiles", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", listFiles(true));
    });

    server.on("/admin/del_file", HTTP_POST, [](AsyncWebServerRequest * request) {
        const char *fileName = request->getHeader("name")->value().c_str();

        if (!SD.exists( fileName)) {
            request->send(400, "text/plain", "ERROR: file does not exist");
        } else {
            File file = SD.open(fileName);
            Serial.println(file.name());
            SD.remove(fileName);
            request->send(200, "text/plain", "Deleted File: " + String(fileName));
        }
    });

    server.on("/admin/set_sd_cursor", HTTP_POST, [](AsyncWebServerRequest * request) {
        sdCursor = request->getHeader("value")->value().c_str();
        request->send(200, "text/plain", "");
    });

    server.on("/admin/get_sd_cursor", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", sdCursor);
    });

    server.on("/admin/reset", HTTP_POST,[](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->addHeader("Access-Control-Allow-Origin","*");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, data");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET");
        response->setCode(200);
        request->send(response);

        delay(500);

        ESP.restart();
    });
}

void setup() {
    // write your initialization code here
    Serial.begin(115200);

    auto allLights = new light[NUM_LEDS];

    for (int i = 0; i < NUM_LEDS; i++) {
        auto lamp = new light();
        lamp->mapped = i;
        lamp->color = 0;
        allLights[i] = *lamp;
    }

    auto startAnimationObject = new Animations(allLights);
    auto startCluster = new LightCluster(allLights, NUM_LEDS, 0, startAnimationObject);

    clusters[0] = startCluster;
    numClusters = 1;
    updateOccupiedLamps();

    Serial.println("\nBooted. Connecting SD card\n");

    connectSDCard();
    Serial.println("\nSD card connected. Connecting WiFi\n");
    delay(250);

    connectWiFi();
    Serial.println("\nWifi connected. Setting up server\n");
    delay(250);

    setupServer();
    delay(250);
    server.begin();
    Serial.println("\nServer started. Connecting to IR Receiver\n");
    delay(250);

    IrReceiver.begin(IR_RECEIVE_PIN, false);
    Serial.println("\nIR Receiver connected. Starting LEDS\n");

    //LightCluster myCluster(lampsInCluster1, 7, 0, Animations(lampsInCluster1));

    delay(1000);
    leds.begin();
    leds.clear();
    //leds.setPixelColor(1, 255);
    leds.show();

    Serial.println("Started LEDS");

    //int lampsInCluster1[] = {0,1,2,3,4,5,6,7,8,9};
    //LightCluster mycluserr(lampsInCluster1, 0, 0);
    //clusters[0] = LightCluster(lampsInCluster1, 0, 0);

    Serial.println("Cluster made");

    delay(250);
    Serial.println("Setup done");
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

    if (lastIrReceive < millis() - 500 && IrReceiver.decode()) {
        lastIrReceive = millis();
        IrReceiver.resume();
        Serial.print("Got ir command ");
        Serial.println(IrReceiver.decodedIRData.command);
        if (IrReceiver.decodedIRData.command == 0x10) {
            // do something
        } else if (IrReceiver.decodedIRData.command == 0x11) {
            // do something else
        }
    }

    bool hasRun = false;
    for (int i = 0; i < numClusters; i++) {
        hasRun = hasRun || clusters[i]->runAnimation();
    }
    if (hasRun || extShow) {
        noInterrupts();
        leds.show();
        interrupts();
        extShow = false;
    }

    /*Serial.print("Num clusters: ");
    Serial.println(numClusters);

    Serial.print("Num occupied lamps: ");
    Serial.println(numOccupiedLamps);


    if (numClusters > 0) {
        Serial.println("");
        for (int c = 0; c < numClusters; c++) {
            for (int i = 0; i < clusters[c]->numLights; i++) {
                Serial.print("Mapped light: ");
                Serial.println(clusters[c]->lights[i].mapped);
            }
            delay(1000);
        }
    }*/
    delay(1);
}
