#include <WString.h>

#ifndef MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H
#define MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H


class SECRETS
{
public:
    static const char *MQTT_SERVER;                         // Broker address

    static const char *MQTT_USER;                                  // Username to connect to your MQTT broker
    static const char *MQTT_PASSWORD;                              // Password to connect to your MQTT broker
    static const char *MQTT_ID;                                    // Name of our device, must be unique

    static const char *TOPIC_PERIPHERAL_OUT;
    static const char *TOPIC_CONTROLLER_COMMANDS;

    static const char *TOPIC_GET_INFO_ALL;
    static const char *TOPIC_DEBUG_OUT;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H
