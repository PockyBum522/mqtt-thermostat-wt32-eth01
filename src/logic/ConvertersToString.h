
#ifndef MQTT_THERMOSTAT_WT32_ETH01_CONVERTERSTOSTRING_H
#define MQTT_THERMOSTAT_WT32_ETH01_CONVERTERSTOSTRING_H

#include <Arduino.h>
#include "models/StateEnums/ThermostatModeStates.h"
#include "models/StateEnums/FanStates.h"

class ConvertersToString
{

public:
    static String getThermostatModeAsString(ThermostatModeStates thermostatMode);

    static String getFanModeAsString(FanStates fanMode);
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CONVERTERSTOSTRING_H
