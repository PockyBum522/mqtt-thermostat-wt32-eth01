
#ifndef MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H
#define MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H

#include <Arduino.h>
#include "PubSubClient.h"
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"

class MqttLogistics
{
public:
    explicit MqttLogistics(CurrentThermostatStatus* currentThermostatStatus, WiFiClient* client);

    void onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length);

    void reconnectMqttIfNotConnected();

    void publish(const char *topic, const char *payload);

    void loopClient();

    std::string getLastCommand();

private:
    CurrentThermostatStatus* _currentThermostatStatus;
    WiFiClient* _ethernetClient;
    PubSubClient* _mqttClient;

    std::string _lastCommand;

};

#endif //MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H
