
#ifndef MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H
#define MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H


#include <Arduino.h>

class MqttLogistics
{
public:
    static void callback(char* topic, byte* payload, unsigned int length);

    static void reconnectMqttIfNotConnected();

    void yieldToMqttWork();

    static void reconnect();
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_MQTTLOGISTICS_H
