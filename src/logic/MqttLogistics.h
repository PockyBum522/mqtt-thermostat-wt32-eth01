
#ifndef MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H
#define MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H

#include <Arduino.h>
#include "PubSubClient.h"

class MqttLogistics
{
public:
    explicit MqttLogistics(const WiFiClient& ethernetClient);

    void callback(char* topic, byte* payload, unsigned int length);

    void ReconnectMqttIfNotConnected();

    void reconnect();

    void publish(const char *topic, const char *payload);

private:
    PubSubClient _mqttClient;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H
