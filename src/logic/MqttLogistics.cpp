#include <ETH.h>
#include "SECRETS.h"
#include "MqttLogistics.h"
#include "PubSubClient.h"
#include "WebServer_WT32_ETH01.h"

MqttLogistics::MqttLogistics(const WiFiClient& ethernetClient)
{
    _mqttClient = *new PubSubClient(SECRETS::MQTT_SERVER, 1883, (Client &)ethernetClient);
}

void MqttLogistics::callback(char* topic, byte* payload, unsigned int length)
{
    String payloadStr = "";

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    for (unsigned int i = 0; i < length; i++)
    {
        payloadStr += ((char)payload[i]);
    }

    // Handle getInfoAllNodes request
    if (payloadStr.equalsIgnoreCase(F("getInfoAllNodes")))
    {
        String messageToSend = String(SECRETS::MQTT_ID) + " is up at IPv4: " + ETH.localIP().toString();
        _mqttClient.publish(SECRETS::TOPIC_GET_INFO_ALL, messageToSend.c_str());
    }
}

void MqttLogistics::ReconnectMqttIfNotConnected()
{
    if (!_mqttClient.connected())
    {
        reconnect();
    }
}

void MqttLogistics::reconnect()
{
    // Loop until we're reconnected
    while (!_mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(SECRETS::MQTT_SERVER);

        // Attempt to connect
        if (_mqttClient.connect("arduino", SECRETS::MQTT_USER, SECRETS::MQTT_PASSWORD))
        {
            Serial.println("...connected");

            // Once connected, publish an announcement...
            String data = "Hello from WT32_ETH01_Thermostat on " + String(BOARD_NAME) + ", at IPv4: " + ETH.localIP().toString();
            _mqttClient.publish(SECRETS::TOPIC_GET_INFO_ALL, data.c_str());

            // ... and resubscribe
            _mqttClient.subscribe(SECRETS::TOPIC_PERIPHERAL_OUT);
            _mqttClient.subscribe(SECRETS::TOPIC_CONTROLLER_COMMANDS);
            _mqttClient.subscribe(SECRETS::TOPIC_GET_INFO_ALL);
        }
        else
        {
            Serial.print("...failed, rc=");
            Serial.print(_mqttClient.state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttLogistics::publish(const char* topic, const char* payload)
{
    _mqttClient.publish(topic, payload);
}
