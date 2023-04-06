#include <Arduino.h>
#include <WebServer_WT32_ETH01.h>         // https://github.com/khoih-prog/WebServer_WT32_ETH01/
#include <AsyncTCP.h>                     // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>            // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>              // https://github.com/ayushsharma82/AsyncElegantOTA
#include <ArduinoJson.h>                 // PubSubClient - https://pubsubclient.knolleary.net/api
#include "logic/MqttLogistics.h"
#include "models/ThermostatReport.h"
#include "secrets.h"

//#define _ETHERNET_WEBSERVER_LOGLEVEL_       3            // Debug Level from 0 to 4

#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial
#define USE_MOCK_SHT3X_VALUE                             // If defined, will fake temp/humidity readings and not need SHT3X sensor connected for testing

#define SECONDS 1000

#define SECONDS_COMPRESSOR_TIMEOUT (900 * SECONDS)
#define SECONDS_PERIPHERAL_MESSAGE_SEND_INTERVAL (15 * SECONDS)

#define PIN_GREEN_FAN_CALL 4              // relayBoardIn01 =  IO4;
#define PIN_WHITE_AUX_HEAT 12             // relayBoardIn03 = IO12;
#define PIN_ORANGE_REVERSER_VALVE 14      // relayBoardIn04 = IO14;
#define PIN_YELLOW_COMPRESSOR_CALL 15     // relayBoardIn05 = IO15;

unsigned long LastPeripheralOutMessageSentSeconds = 0;
unsigned long LastCompressorOffSeconds = 0;                 // Set this when compressor turns off

float CurrentTemperatureFarenheit = 0.0;
float CurrentHumidity = 0.0;

unsigned long CurrentSeconds = 0;
String CurrentThermostatMode = "Uninitialized";

char MqttMessageBuffer[150]; // Buffer for incoming MQTT message

AsyncWebServer WebServer(80);
WiFiClient EthernetClient;

MqttLogistics mqttLogistics = *new MqttLogistics(EthernetClient);

unsigned long lastMsg = 0;

void reportTemperatureEveryTimeout(int response);

void printSerialDebugMessages();

String SerializeReportToSend(const ThermostatReport& report);

int getShtRealOrMockValue();

void setup()
{
    pinMode(PIN_GREEN_FAN_CALL, OUTPUT);
    pinMode(PIN_WHITE_AUX_HEAT, OUTPUT);
    pinMode(PIN_ORANGE_REVERSER_VALVE, OUTPUT);
    pinMode(PIN_YELLOW_COMPRESSOR_CALL, OUTPUT);

    digitalWrite(PIN_GREEN_FAN_CALL, LOW);
    digitalWrite(PIN_WHITE_AUX_HEAT, LOW);
    digitalWrite(PIN_ORANGE_REVERSER_VALVE, LOW);
    digitalWrite(PIN_YELLOW_COMPRESSOR_CALL, LOW);

    Serial.begin(115200);

    Serial.print("\nStarting WT32_ETH01_Thermostat on " + String(ARDUINO_BOARD));
    Serial.println(" with " + String(SHIELD_TYPE));
    Serial.println(WEBSERVER_WT32_ETH01_VERSION);

    // To be called before ETH.begin()
    //WT32_ETH01_onEvent();

    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

    // Static IP, leave without this line to get IP via DHCP
    // ETH.config(myIP, myGW, mySN, myDNS);

    //WT32_ETH01_waitForConnect();

    // Note - the default maximum packet size is 128 bytes. If the
    // combined length of clientId, username and password exceed this use the
    // following to increase the buffer size:
    // client.setBufferSize(255);

    WebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hi! I am WT32_ETH01_Thermostat.");
    });

    AsyncElegantOTA.begin(&WebServer);    // Start ElegantOTA
    WebServer.begin();

    Serial.println();
    Serial.println("HTTP server started with MAC: " + ETH.macAddress() + ", at IPv4: " + ETH.localIP().toString());
    Serial.println();
}

void loop()
{
    CurrentSeconds = millis() / SECONDS;

    mqttLogistics.ReconnectMqttIfNotConnected();

    int shtResponse = getShtRealOrMockValue();
    reportTemperatureEveryTimeout(shtResponse);

    printSerialDebugMessages();

    delay(1000);
}

void handleCurrentThermostatMode()
{
    // if (thermostatMode == 0)
    // {
    //   if (thermostatMode != lastMode)
    //   {
    //     // We just changed modes to 0 - All off

    //     turnOffFan();
    //     turnOffCompressor();
    //     turnOffAuxHeat();

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // }
    // else if (thermostatMode == 1)
    // {
    //   // If compressor couldn't be turned on because of timeout, check it again
    //   if (waitingForCompressorTimeout){ turnOnCompressorForCool(); }

    //   if (thermostatMode != lastMode)
    //   {
    //     // We just changed modes to 51 - Cool override

    //     turnOnFan();
    //     turnOnCompressorForCool(); // This will also call the reversing valve right before the compressor turns on
    //     turnOffAuxHeat();

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // }
    // else if (thermostatMode == 2)
    // {
    //   // If compressor couldn't be turned on because of timeout, check it again
    //   if (waitingForCompressorTimeout){ turnOnCompressorForHeat(); }

    //   if (thermostatMode != lastMode){
    //     // We just changed modes to 2 - Heatpumpheat

    //     turnOnFan();
    //     turnOnCompressorForHeat(); // This will also call the reversing valve right before the compressor turns on
    //     turnOffAuxHeat();

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // }
    // else if (thermostatMode == 6)
    // {
    //   if (thermostatMode != lastMode)
    //   {
    //     // We just changed modes to 6 - Fan only

    //     turnOnFan();
    //     turnOffCompressor(); // This will also call the reversing valve right before the compressor turns on
    //     turnOffAuxHeat();

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // }

    // // Manual override: All off until reset
    // if (thermostatMode == 50){
    //   if (thermostatMode != lastMode){
    //     // We just changed modes to 50 - All off override

    //     turnOffFan();
    //     turnOffCompressor();
    //     turnOffAuxHeat();

    //     waitingForCompressorTimeout = false;

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // } else if (thermostatMode == 51){

    //   // If compressor couldn't be turned on because of timeout, check it again
    //   if (waitingForCompressorTimeout){ turnOnCompressorForCool(); }

    //   if (thermostatMode != lastMode){
    //     // We just changed modes to 51 - Cool override

    //     turnOnFan();
    //     turnOnCompressorForCool(); // This will also call the reversing valve right before the compressor turns on
    //     turnOffAuxHeat();

    //     // So this only happens once
    //     lastMode = thermostatMode;
    //   }
    // }
}

void printSerialDebugMessages()
{
    Serial.print("Now (seconds):\t");
    Serial.print(CurrentSeconds);

    Serial.print(" | LastCompressorOffSeconds:\t");
    Serial.print(LastCompressorOffSeconds);

    Serial.print(" | LastPeripheralOutMessageSentSeconds:\t");
    Serial.print(LastPeripheralOutMessageSentSeconds);

    unsigned long secondsSinceLastReportSent = CurrentSeconds - LastPeripheralOutMessageSentSeconds;

    Serial.print(" | secondsSinceLastReportSent:\t");
    Serial.print(secondsSinceLastReportSent);

    Serial.println();
}

void reportTemperatureEveryTimeout(int shtResponse)
{
    unsigned long secondsSinceLastReportSent = CurrentSeconds - LastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > 15)
    {
        LastPeripheralOutMessageSentSeconds = CurrentSeconds;                 // Reset time since last message

        //Serial.println("Sht30 response code: " [ shtResponse]);

        ThermostatReport reportToSend = *new ThermostatReport();

        if(shtResponse == 0)
        {
            reportToSend.CurrentTemperature = CurrentTemperatureFarenheit;
            reportToSend.CurrentHumidity = CurrentHumidity;
            reportToSend.CompressorLastOffSeconds = LastCompressorOffSeconds;
            reportToSend.CurrentSeconds = CurrentSeconds;
            reportToSend.ThermostatMode = CurrentThermostatMode;
        }
        else
        {
            reportToSend.ThermostatMode = "ERROR_TEMPERATURE_SENSOR";
        }

        String outputJson = SerializeReportToSend(reportToSend);

        mqttLogistics.publish(SECRETS::TOPIC_PERIPHERAL_OUT, outputJson.c_str());

        Serial.print("Sent report: ");
        Serial.println(outputJson);
    }
}

int getShtRealOrMockValue()
{
    int shtResponse = 0;

#ifdef USE_MOCK_SHT30X_VALUE
    CurrentTemperature = 1.2;
    CurrentHumidity = 3.4;
#else

    //shtResponse = _sht30.get();
#endif

    return shtResponse;
}

String SerializeReportToSend(const ThermostatReport& reportToSend)
{
    StaticJsonDocument<456> jsonDocument;

    jsonDocument["CurrentTemperature"] = reportToSend.CurrentTemperature;
    jsonDocument["CurrentHumidity"] = reportToSend.CurrentHumidity;
    jsonDocument["CompressorLastOffSeconds"] = reportToSend.CompressorLastOffSeconds;
    jsonDocument["CurrentSeconds"] = reportToSend.CurrentSeconds;
    jsonDocument["ThermostatMode"] = reportToSend.ThermostatMode;

    String outputJson = "";

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}

// void turnOnFan()
// {
//   digitalWrite(PIN_GREEN_FAN_CALL, LOW);
//   client.publish(topicPubSub, "status:FanOn");
// }

// void turnOffFan()
// {
//   digitalWrite(PIN_GREEN_FAN_CALL, HIGH);
//   client.publish(topicPubSub, "status:FanOff");
// }

// void turnOnCompressorForCool()
// {
//   if (_nowSeconds - _lastCompressorOffSeconds > secondsCompressorMustBeOff){

//     turnOnReverserValve();

//     digitalWrite(compressorYellow, LOW);

//     client.publish(topicPubSub, "status:CompressorOn");
//   } else {
//     client.publish(topicPubSub, "warn:CompressorTimeoutElapsing");
//   }
// }

// void turnOnCompressorForHeat()
// {
//   if (_now - _lastMillisCompressorOff > secondsCompressorMustBeOff){

//     digitalWrite(compressorYellow, LOW);

//     client.publish(topicPubSub, "status:CompressorOn");

//     turnOffReverserValve();
//   } else {
//     client.publish(topicPubSub, "error:CompressorTimeoutElapsing");
//   }
// }

// void turnOffCompressor()
// {
//   digitalWrite(compressorYellow, HIGH);

//   client.publish(topicPubSub, "status:CompressorOff");

//   waitingForCompressorTimeout = false;
// }

// void turnOnAuxHeat()
// {
//   digitalWrite(auxHeatWhite, LOW);
//   client.publish(topicPubSub, "status:AuxHeatOn");
// }

// void turnOffAuxHeat()
// {
//   digitalWrite(auxHeatWhite, HIGH);
//   client.publish(topicPubSub, "status:AuxHeatOff");
// }

// void turnOnReverserValve()
// {
//   digitalWrite(reverserValveOrange, LOW);
//   client.publish(topicPubSub, "status:ReverserValveOn");
// }

// void turnOffReverserValve()
// {
//   digitalWrite(reverserValveOrange, HIGH);
//   client.publish(topicPubSub, "status:ReverserValveOff");
// }