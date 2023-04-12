#ifndef MQTT_THERMOSTAT_WT32_ETH01_FAN_MODE_HANDLER_H
#define MQTT_THERMOSTAT_WT32_ETH01_FAN_MODE_HANDLER_H


#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"

class FanModeHandler
{
public:
    explicit FanModeHandler(CurrentThermostatStatus *currentThermostatStatus);

    void setFanStateBasedOnSystemStatus();

    void turnFanOn();

    void turnFanOff();

private:
    CurrentThermostatStatus *_currentThermostatStatus;

    bool _lastFanState = STATE_OFF;

    bool getNextFanState();
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_FAN_MODE_HANDLER_H
