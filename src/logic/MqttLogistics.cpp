//
// Created by David on 4/6/2023.
//

#include "MqttLogistics.h"

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
        _client.publish(_topicGetInfo, messageToSend.c_str());
    }
}

void MqttLogistics::reconnectMqttIfNotConnected()
{
    if (!_client.connected())
    {
        reconnect();
    }
}

void MqttLogistics::yieldToMqttWork()
{
    _client.loop();
}

void MqttLogistics::reconnect()
{
    // Loop until we're reconnected
    while (!_client.connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(_mqttServer);

        // Attempt to connect
        if (_client.connect("arduino", _mqttBrokerUser, _mqttBrokerPass))
        {
            Serial.println("...connected");

            // Once connected, publish an announcement...
            String data = "Hello from WT32_ETH01_Thermostat on " + String(BOARD_NAME) + ", at IPv4: " + ETH.localIP().toString();
            _client.publish(_topicPubSub, data.c_str());

            // ... and resubscribe
            _client.subscribe(_topicPubSub);
            _client.subscribe(_topicTempSet);
            _client.subscribe(_topicGetInfo);
        }
        else
        {
            Serial.print("...failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
