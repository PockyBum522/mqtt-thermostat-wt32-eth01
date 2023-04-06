#include <Arduino.h>

#ifndef MQTT_THERMOSTAT_WT32_ETH01_THERMOSTATREPORT_H
#define MQTT_THERMOSTAT_WT32_ETH01_THERMOSTATREPORT_H

class ThermostatReport {
    public:
        float CurrentTemperature{};
        float CurrentHumidity{};

        float CurrentTemperatureSetpoint{};

        unsigned long CompressorLastOffSeconds{};
        unsigned long CurrentSeconds{};

        String ThermostatMode{};
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_THERMOSTATREPORT_H

