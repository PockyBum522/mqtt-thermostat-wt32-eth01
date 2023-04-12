#ifndef MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_RELAYS_HANDLER_H
#define MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_RELAYS_HANDLER_H


#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "models/PinDefinitions.h"
#include "models/Enums/ThermostatInternalModeEnum.h"

class ThermostatModeHandler
{
public:
    explicit ThermostatModeHandler(CurrentThermostatStatus* currentThermostatStatus);

    void handleCurrentMode();

private:
    CurrentThermostatStatus* _currentThermostatStatus;

    void handleFanModeChange();

    void handleCurrentCompressorMode();

    void handleReverserValveChange();

    void turnAllThermostatRelaysOff();

    void turnOnCompressorWithTimeout();

    unsigned long _lastModeCheckAtSeconds = 120;

    FanModeEnum _lastFanMode = FanOff;
    ThermostatModeEnum _lastThermostatMode = ThermostatUninitialized;

    ThermostatInternalModeEnum _currentThermostatInternalMode = InternalUninitialized;
    ThermostatInternalModeEnum _lastThermostatInternalMode = InternalUninitialized;;

    bool _lastReverserValveState = STATE_OFF;
    bool _lastCompressorState = STATE_OFF;

    void setAllRelaysToNewStates();
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_RELAYS_HANDLER_H
