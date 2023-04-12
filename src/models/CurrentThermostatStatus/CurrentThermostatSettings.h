//
// Created by David on 4/12/2023.
//

#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H


class CurrentThermostatSettings
{
public:
    // TODO: CHANGE THIS TO 900 WHEN USING FOR REAL
    const int CompressorTimeoutSeconds = 20;
    const int DebugMessageSendIntervalSeconds = 3;
    const int TemperatureReportMessageSendIntervalSeconds = 5;

    // How far to overshoot the setpoint/let the temp get back to when the house returns and goes past setpoint the other direction
    const float MarginFahrenheit = 0.75;

    const int ModeCheckIntervalSeconds = 60;

    const bool DebugModeOn = false;
    const bool UseMockTemperatureSensorData = false;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
