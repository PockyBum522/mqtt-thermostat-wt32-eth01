#ifndef MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_MODE_ENUM_H
#define MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_MODE_ENUM_H

enum ThermostatModeEnum
{
    ThermostatUninitialized,
    ThermostatCool,
    ThermostatHeat,
    ThermostatMaintainRange,
    ThermostatEmergencyHeat,
    ThermostatOff,
    ErrorSensorTemperatureProblem

};

#endif //MQTT_THERMOSTAT_WT32_ETH01_THERMOSTAT_MODE_ENUM_H
