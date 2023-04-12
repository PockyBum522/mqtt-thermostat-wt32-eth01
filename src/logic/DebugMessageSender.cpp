#include "DebugMessageSender.h"
#include "MqttLogistics.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"

DebugMessageSender::DebugMessageSender(CurrentThermostatStatus* currentThermostatStatus,
                                       MqttLogistics* mqttLogistics)
{
    DebugMessageSender::_currentThermostatStatus = currentThermostatStatus;
    DebugMessageSender::_mqttLogistics = mqttLogistics;
}

void DebugMessageSender::SendMqttDebugMessagesEveryTimeout()
{
    unsigned long secondsSinceLastDebugMessageSent = DebugMessageSender::_currentThermostatStatus->CurrentSeconds - _lastDebugOutMessageSentSeconds;

    if (secondsSinceLastDebugMessageSent > DebugMessageSender::_currentThermostatStatus->CurrentSettings.DebugMessageSendIntervalSeconds)
    {
        DebugMessageSender::_lastDebugOutMessageSentSeconds = DebugMessageSender::_currentThermostatStatus->CurrentSeconds;

        StaticJsonDocument<356> jsonDocument;

        jsonDocument["CurrentSeconds"] = DebugMessageSender::_currentThermostatStatus->CurrentSeconds;

        jsonDocument["LastSeenMqttCommand"] = _mqttLogistics->getLastCommand();

        jsonDocument["secondsSinceLastDebugMessageSent"] = secondsSinceLastDebugMessageSent;

        jsonDocument["Relay01Status"] = !digitalRead(PIN_RELAY_01);
        jsonDocument["Relay02Status"] = !digitalRead(PIN_RELAY_02);
        jsonDocument["Relay03Status"] = !digitalRead(PIN_RELAY_03);
        jsonDocument["Relay04Status"] = !digitalRead(PIN_RELAY_04);
        jsonDocument["Relay05Status"] = !digitalRead(PIN_RELAY_05);
        jsonDocument["Relay06Status"] = !digitalRead(PIN_RELAY_06);

        String outputJson = "";

        serializeJson(jsonDocument, outputJson);

        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_OUT, outputJson.c_str());

        if (DebugMessageSender::_currentThermostatStatus->CurrentSettings.DebugModeOn)
        {
            Serial.println("Sent debug message");
            Serial.println(outputJson);
        }
    }
}
