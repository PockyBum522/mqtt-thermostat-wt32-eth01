#include <sstream>
#include <iomanip>
#include "TemperatureReporter.h"
#include "MqttLogistics.h"
#include "secrets/SECRETS.h"
#include "../../lib/ArduinoJson-6.x/src/ArduinoJson/Document/StaticJsonDocument.hpp"

TemperatureReporter::TemperatureReporter(CurrentThermostatStatus* currentThermostatStatus,
                                         MqttLogistics* mqttLogistics)
{
    _currentThermostatStatus = currentThermostatStatus;
    _mqttLogistics = mqttLogistics;
}

void TemperatureReporter::SendTemperatureReportEveryTimeout()
{
    unsigned long secondsSinceLastReportSent = TemperatureReporter::_currentThermostatStatus->CurrentSeconds - _lastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > TemperatureReporter::_currentThermostatStatus->CurrentSettings.TemperatureReportMessageSendIntervalSeconds)
    {
        _lastPeripheralOutMessageSentSeconds = _currentThermostatStatus->CurrentSeconds;                 // Reset time since last message

        std::string outputJson = TemperatureReporter::SerializeReport();

        _mqttLogistics->publish(SECRETS::TOPIC_PERIPHERAL_OUT, outputJson.c_str());

        char temperatureBuffer[7];
        char setpointBuffer[7];

        String currentMode =  getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);

        dtostrf(_currentThermostatStatus->CurrentTemperatureFahrenheit, 5, 2, temperatureBuffer);
        dtostrf(_currentThermostatStatus->CurrentSetpoint, 5, 2, setpointBuffer);

        _mqttLogistics->publish(SECRETS::TOPIC_JUST_TEMPERATURE_PERIPHERAL_OUT, temperatureBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, currentMode.c_str());

        if (TemperatureReporter::_currentThermostatStatus->CurrentSettings.DebugModeOn)
        {
            Serial.print("Sent report: ");
            Serial.println(outputJson.c_str());
        }
    }
}

std::string TemperatureReporter::SerializeReport()
{
    StaticJsonDocument<256> jsonDocument;

    std::stringstream temperatureStream;
    temperatureStream << std::fixed << std::setprecision(2) << _currentThermostatStatus->CurrentTemperatureFahrenheit;
    std::string outTemperatureString = temperatureStream.str();

    std::stringstream humidityStream;
    humidityStream << std::fixed << std::setprecision(2) << _currentThermostatStatus->CurrentHumidity;
    std::string outHumidityString = humidityStream.str();

    jsonDocument["TemperatureFahrenheit"] = outTemperatureString;
    jsonDocument["HumidityPercentage"] = outHumidityString;

    jsonDocument["Setpoint"] = _currentThermostatStatus->CurrentSetpoint;

    jsonDocument["ThermostatMode"] = getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);
    jsonDocument["FanMode"] = getFanModeAsString(_currentThermostatStatus->FanMode);

    jsonDocument["CompressorDelayIfNegative"] = (long)((long)_currentThermostatStatus->CurrentSeconds - (long)_currentThermostatStatus->LastCompressorOffAtSeconds) - (long)_currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;
    jsonDocument["CurrentSeconds"] = _currentThermostatStatus->CurrentSeconds;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}

String TemperatureReporter::getThermostatModeAsString(ThermostatModeStates thermostatMode)
{
    switch (thermostatMode)
    {
        case ModeUninitialized:
            return "uninitialized";

        case ModeOff:
            return "off";

        case ModeCooling:
            return "cool";

        case ModeMaintainingRange:
            return "Maintaining Range";

        case ModeHeating:
            return "heat";
    }

    return "ERROR";
}

String TemperatureReporter::getFanModeAsString(FanStates fanMode)
{
    switch (fanMode)
    {
        case FanUninitialized:
            return "Uninitialized";

        case FanAlwaysOn:
            return "Always On";

        case FanOnAutomatically:
            return "Automatic";

        case FanAlwaysOff:
            return "Always Off";
    }

    return "ERROR";
}
