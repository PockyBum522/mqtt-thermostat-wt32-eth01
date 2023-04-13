#include <Arduino.h>
#include <WiFiClient.h>
#include <WebServer_WT32_ETH01.h>         // https://github.com/khoih-prog/WebServer_WT32_ETH01/
#include <ESPAsyncWebServer.h>            // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>              // https://github.com/ayushsharma82/AsyncElegantOTA
#include "logic/Sht3xHandler.h"
#include <Wire.h>
#include <sstream>
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "logic/MqttLogistics.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"
#include "logic/TemperatureReporter.h"
#include "logic/DebugMessageSender.h"
#include "logic/ThermostatStateMachine.h"

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

bool initialSettingsSet = false;

AsyncWebServer webServer(80);
WiFiClient ethernetClient;

CurrentThermostatStatus currentStatus = *new CurrentThermostatStatus();

MqttLogistics mqttLogistics = *new MqttLogistics(&currentStatus, &ethernetClient);

auto temperatureReportSender = *new TemperatureReporter(&currentStatus, &mqttLogistics);
auto debugMqttSender = *new DebugMessageSender(&currentStatus, &mqttLogistics);
auto temperatureSensorHandler = *new Sht3xHandler(&currentStatus);
auto hvacController = *new HvacController(&currentStatus, &mqttLogistics);
auto thermostatStateMachine = *new ThermostatStateMachine(&currentStatus, &hvacController, &mqttLogistics);

void setInitialSettingsAfterDelay();

void startWifi();

void startEthernet();

void setup()
{
    pinMode(PIN_RELAY_01, OUTPUT);
    pinMode(PIN_RELAY_02, OUTPUT);
    pinMode(PIN_RELAY_03, OUTPUT);
    pinMode(PIN_RELAY_04, OUTPUT);
    pinMode(PIN_RELAY_05, OUTPUT);
    pinMode(PIN_RELAY_06, OUTPUT);

    // Turn all pins off
    digitalWrite(PIN_RELAY_01, HIGH);
    digitalWrite(PIN_RELAY_02, HIGH);
    digitalWrite(PIN_RELAY_03, HIGH);
    digitalWrite(PIN_RELAY_04, HIGH);
    digitalWrite(PIN_RELAY_05, HIGH);
    digitalWrite(PIN_RELAY_06, HIGH);

    Serial.begin(115200);

    Serial.print("\nStarting WT32_ETH01_Thermostat on " + String(ARDUINO_BOARD));
    Serial.println(" with " + String(SHIELD_TYPE));
    Serial.println(WEBSERVER_WT32_ETH01_VERSION);

    if (currentStatus.CurrentSettings.UseWiFiInsteadOfEthernetForTesting)
    {
        startWifi();
        mqttLogistics = *new MqttLogistics(&currentStatus, reinterpret_cast<WiFiClient *>(&WiFi));
    }
    else
    {
        startEthernet();
    }

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", "Hi! I am WT32_ETH01_Thermostat.");
    });

    AsyncElegantOTA.begin(&webServer);    // Start ElegantOTA
    webServer.begin();

    Serial.println();
    Serial.println("V03_HTTP server started with MAC: " + ETH.macAddress() + ", at IPv4: " + ETH.localIP().toString());
    Serial.println();
}

void loop()
{
    temperatureSensorHandler.UpdateCurrentStatus();

    mqttLogistics.reconnectMqttIfNotConnected();
    mqttLogistics.loopClient();

    thermostatStateMachine.loop();
    hvacController.loopForQueuedCompressorHandling();

    temperatureReportSender.SendTemperatureReportEveryTimeout();
    debugMqttSender.SendMqttDebugMessagesEveryTimeout();

    setInitialSettingsAfterDelay(); // Fires once after 30 seconds to set sane settings after brownouts
}

void setInitialSettingsAfterDelay()
{
    if (!(millis() > 30000 && !initialSettingsSet))
        return;

    initialSettingsSet = true;

    currentStatus.ThermostatMode = ModeCooling;
    currentStatus.FanMode = FanOnAutomatically;
    currentStatus.CurrentSetpoint = 71.0;
}

void startEthernet()
{
    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

    // Static IP, leave without this line to get IP via DHCP
    // ETH.config(myIP, myGW, mySN, myDNS);
}

void startWifi()
{
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(SECRETS::WIFI_SSID, SECRETS::WIFI_PSK);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}