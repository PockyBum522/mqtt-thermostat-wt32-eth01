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

    std::string currentThermostatModeToString = ConvertCurrentThermostatModeToString(TemperatureReporter::_currentThermostatStatus->CurrentThermostatMode);
    std::string currentFanModeToString = ConvertCurrentFanModeToString(TemperatureReporter::_currentThermostatStatus->CurrentFanMode);

    jsonDocument["CurrentTemperatureFahrenheit"] = outTemperatureString;
    jsonDocument["CurrentHumidity"] = outHumidityString;

    jsonDocument["ThermostatMode"] = currentThermostatModeToString;
    jsonDocument["FanMode"] = currentFanModeToString;

    unsigned long lastCompressorOffAtSeconds = _currentThermostatStatus->LastCompressorOffAtSeconds;
    unsigned long currentSeconds = _currentThermostatStatus->CurrentSeconds;
    jsonDocument["CompressorOnInSeconds"] = currentSeconds - lastCompressorOffAtSeconds;

    jsonDocument["CurrentSeconds"] = TemperatureReporter::_currentThermostatStatus->CurrentSeconds;
    jsonDocument["CurrentSetpoint"] = TemperatureReporter::_currentThermostatStatus->CurrentSetpoint;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}

std::string TemperatureReporter::ConvertCurrentThermostatModeToString(ThermostatModeEnum currentThermostatMode)
{
    switch (currentThermostatMode)
    {
        case ThermostatUninitialized:
            return "Uninitialized";

        case ThermostatCool:
            return "Cool";

        case ThermostatHeat:
            return "Heat";

        case ThermostatEmergencyHeat:
            return "Emergency Heat";

        case ThermostatMaintainRange:
            return "Maintain Range";

        case ThermostatOff:
            return "Off";

        case ErrorSensorTemperatureProblem:
            return "ERROR: TEMPERATURE SENSOR COMMUNICATION PROBLEM";

    }
}

std::string TemperatureReporter::ConvertCurrentFanModeToString(FanModeEnum currentFanMode)
{
    switch (currentFanMode)
    {
        case FanUninitialized:
            return "Uninitialized";

        case FanOff:
            return "Off";

        case FanOn:
            return "On";
    }
}
