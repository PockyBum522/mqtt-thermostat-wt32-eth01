
#ifndef MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H
#define MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H

#include <WiFiClient.h>
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "../../lib/PubSubClient/src/PubSubClient.h"

class MqttLogistics
{
public:
    explicit MqttLogistics(CurrentThermostatStatus *currentThermostatStatus, WiFiClient *client);

    void onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length);

    void reconnectMqttIfNotConnected();

    void publish(const char *topic, const char *payload);

    void loopClient();

    std::string getLastCommand();

private:
    CurrentThermostatStatus *_currentThermostatStatus;
    WiFiClient *_ethernetClient;
    PubSubClient *_mqttClient;

    std::string _lastCommand;

    void setThermostatMode(const String &payloadStr);
    void setThermostatSetpoint(String commandString);

    static String getIncomingPayloadAsString(uint8_t *string, unsigned int payloadLength);

    static String getIncomingTopicAsString(char *topic);

    boolean isNumeric(String str);
};

#endif //MQTT_THERMOSTAT_WT32_ETH01_MQTT_LOGISTICS_H