#include <Arduino.h>
#include <WebServer_WT32_ETH01.h>         // https://github.com/khoih-prog/WebServer_WT32_ETH01/
#include <AsyncTCP.h>                     // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>            // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>              // https://github.com/ayushsharma82/AsyncElegantOTA
#include <WEMOS_SHT3X.h>                  // https://github.com/wemos/WEMOS_SHT3x_Arduino_Library
#include <PubSubClient.h>                 // PubSubClient - https://pubsubclient.knolleary.net/api

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.println("Test loop");

    delay(2000);
}