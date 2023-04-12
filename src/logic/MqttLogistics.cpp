#include <ETH.h>
#include "secrets/SECRETS.h"
#include "models/PinDefinitions.h"
#include "MqttLogistics.h"

MqttLogistics::MqttLogistics(CurrentThermostatStatus* currentThermostatStatus,
                             WiFiClient* client)
{
    _ethernetClient = client;
    _currentThermostatStatus = currentThermostatStatus;

    _mqttClient = new PubSubClient();

    _mqttClient->setClient((Client&)_ethernetClient);
    _mqttClient->setServer(SECRETS::MQTT_SERVER, 1883);
    _mqttClient->setBufferSize(512);

    _mqttClient->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onMqttMessageReceived(topic, payload, length); });
}

void MqttLogistics::onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    // A lot of the arduino string functions are really useful, so we'll use it here
    String payloadStr = "";
    String topicStr = "";

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    // Convert payload to string
    for (int i = 0; i < length; i++)
    {
        payloadStr += ((char)payload[i]);
    }

    Serial.print("Payload is [");
    Serial.print(payloadStr);
    Serial.print("] ");
    Serial.println();

    // Convert topic to string
    for (int i = 0; i < strlen(topic); i++)
    {
        topicStr += ((char)topic[i]);
    }

    // Handle getInfoAllNodes request
    if (payloadStr.equalsIgnoreCase(F("getInfoAllNodes")))
    {
        _lastCommand = payloadStr.c_str();

        String messageToSend = String(SECRETS::MQTT_ID) + " is up at IPv4: " + ETH.localIP().toString();
        MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, messageToSend.c_str());
    }

    Serial.print("topicStr: ");
    Serial.println(topicStr);

    Serial.print("SECRETS::TOPIC_CONTROLLER_COMMANDS:");
    Serial.println(SECRETS::TOPIC_CONTROLLER_COMMANDS);

    Serial.println(topicStr.equalsIgnoreCase(SECRETS::TOPIC_CONTROLLER_COMMANDS));
    Serial.println(payloadStr.startsWith("RELAY_"));

    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_CONTROLLER_COMMANDS) &&
        payloadStr.startsWith("RELAY_"))
    {
        _lastCommand = payloadStr.c_str();

        int pinToUse = 0;
        bool newPinState;

        Serial.println("Payload startsWith valid...");

        // Check what relay number, formatted as RELAY_01_ON or RELAY_02_OFF, etc...
        if (payloadStr[7] == '1')
            pinToUse = PIN_RELAY_01;

        if (payloadStr[7] == '2')
            pinToUse = PIN_RELAY_02;

        if (payloadStr[7] == '3')
            pinToUse = PIN_RELAY_03;

        if (payloadStr[7] == '4')
            pinToUse = PIN_RELAY_04;

        if (payloadStr[7] == '5')
            pinToUse = PIN_RELAY_05;

        if (payloadStr[7] == '6')
            pinToUse = PIN_RELAY_06;

        if (payloadStr.endsWith("ON"))
            newPinState = false; // ESP32 low = pin on

        if (payloadStr.endsWith("OFF"))
            newPinState = true; // ESP32 high = pin off

        digitalWrite(pinToUse, newPinState);

        String messageToSend = "Setting pin: " + String(pinToUse) + " to new state: " + String(newPinState);
        MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, messageToSend.c_str());
        _lastCommand = payloadStr.c_str();
    }
}

void MqttLogistics::reconnectMqttIfNotConnected()
{
    // Loop until we're reconnected
    while (!MqttLogistics::_mqttClient->connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(SECRETS::MQTT_SERVER);

        // Attempt to connect
        if (MqttLogistics::_mqttClient->connect("WT32_ETH01_Thermostat", SECRETS::MQTT_USER, SECRETS::MQTT_PASSWORD))
        {
            Serial.println("...connected");

            // Once connected, publish an announcement...
            String data = "Hello from WT32_ETH01_Thermostat at IPv4: " + ETH.localIP().toString();
          MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, data.c_str());

            // ... and resubscribe
          MqttLogistics::_mqttClient->subscribe(SECRETS::TOPIC_CONTROLLER_COMMANDS);
          MqttLogistics::_mqttClient->subscribe(SECRETS::TOPIC_GET_INFO_ALL);
        }
        else
        {
            Serial.print("Attempting MQTT connection to ");
            Serial.print(SECRETS::MQTT_SERVER);

            Serial.print("...failed, rc=");
            Serial.print(MqttLogistics::_mqttClient->state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttLogistics::publish(const char* topic, const char* payload)
{
    MqttLogistics::_mqttClient->publish(topic, payload);
}

void MqttLogistics::loopClient()
{
    MqttLogistics::_mqttClient->loop();
}

std::string MqttLogistics::getLastCommand()
{
    return _lastCommand;
}