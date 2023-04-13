#include <sstream>
#include <iomanip>
#include "TemperatureReporter.h"
#include "MqttLogistics.h"
#include "secrets/SECRETS.h"

TemperatureReporter::TemperatureReporter(CurrentThermostatStatus* currentThermostatStatus,
                                         MqttLogistics* mqttLogistics)
{
    TemperatureReporter::_currentThermostatStatus = currentThermostatStatus;
    TemperatureReporter::_mqttLogistics = mqttLogistics;
}

void TemperatureReporter::SendTemperatureReportEveryTimeout()
{
    unsigned long secondsSinceLastReportSent = TemperatureReporter::_currentThermostatStatus->CurrentSeconds - _lastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > TemperatureReporter::_currentThermostatStatus->CurrentSettings.TemperatureReportMessageSendIntervalSeconds)
    {
        _lastPeripheralOutMessageSentSeconds = _currentThermostatStatus->CurrentSeconds;                 // Reset time since last message

        std::string outputJson = TemperatureReporter::SerializeReport();

        _mqttLogistics->publish(SECRETS::TOPIC_PERIPHERAL_OUT, outputJson.c_str());

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
    temperatureStream << std::fixed << std::setprecision(2) << TemperatureReporter::_currentThermostatStatus->CurrentTemperatureFahrenheit;
    std::string outTemperatureString = temperatureStream.str();

    std::stringstream humidityStream;
    humidityStream << std::fixed << std::setprecision(2) << TemperatureReporter::_currentThermostatStatus->CurrentHumidity;
    std::string outHumidityString = humidityStream.str();

    jsonDocument["CurrentTemperatureFahrenheit"] = outTemperatureString;
    jsonDocument["CurrentHumidity"] = outHumidityString;

    jsonDocument["CurrentSetpoint"] = TemperatureReporter::_currentThermostatStatus->CurrentSetpoint;

    jsonDocument["ThermostatMode"] = getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);
    jsonDocument["FanMode"] = getFanModeAsString(_currentThermostatStatus->FanMode);

    jsonDocument["CompressorDelayIfNegative"] = (long)((long)_currentThermostatStatus->CurrentSeconds - (long)_currentThermostatStatus->LastCompressorOffAtSeconds) - (long)_currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;
    jsonDocument["CurrentSeconds"] = TemperatureReporter::_currentThermostatStatus->CurrentSeconds;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}

String TemperatureReporter::getThermostatModeAsString(ThermostatModeStates thermostatMode)
{
    switch (thermostatMode)
    {
        case ModeUninitialized:
            return "Uninitialized";

        case ModeOff:
            return "Off";

        case ModeCooling:
            return "Cooling";

        case ModeMaintainingRange:
            return "Maintaining Range";

        case ModeHeating:
            return "Heating";
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
