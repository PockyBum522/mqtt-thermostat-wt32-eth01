#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H

// If defined, will fake temp/Humidity readings and not need Sht3xHandler sensor connected for testing
//#define USE_MOCK_SHT3X_VALUE

#define SECONDS 1000

#include "logic/Sht3xHandler.h"
#include "CurrentThermostatSettings.h"
#include "models/Enums/ThermostatModeEnum.h"
#include "models/Enums/FanModeEnum.h"
#include "models/PinDefinitions.h"
#include "models/Enums/ThermostatInternalModeEnum.h"

class CurrentThermostatStatus
{
public:

    CurrentThermostatSettings CurrentSettings = *new CurrentThermostatSettings();

    unsigned long CurrentSeconds = 0;

    unsigned long LastCompressorOffAtSeconds = 0;                          // This gets set to CurrentSeconds when compressor turns off
    ThermostatModeEnum CurrentThermostatMode = ThermostatUninitialized;
    FanModeEnum CurrentFanMode = FanUninitialized;

    ThermostatInternalModeEnum ThermostatInternalMode = InternalUninitialized;

    bool CurrentReverserValveState = STATE_OFF;
    bool CurrentCompressorState = STATE_OFF;
    bool CurrentFanState = STATE_OFF;

    float CurrentTemperatureFahrenheit = 0.0;
    float CurrentHumidity = 0.0;
    float CurrentSetpoint = 0.0;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
