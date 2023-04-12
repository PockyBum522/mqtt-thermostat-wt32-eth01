#include <ETH.h>
#include "secrets/SECRETS.h"
#include "MqttLogistics.h"

MqttLogistics::MqttLogistics(CurrentThermostatStatus *currentThermostatStatus,
                             WiFiClient *client)
{
    _ethernetClient = client;
    _currentThermostatStatus = currentThermostatStatus;

    _mqttClient = new PubSubClient();

    _mqttClient->setClient(*_ethernetClient);
    _mqttClient->setServer(SECRETS::MQTT_SERVER, 1883);
    _mqttClient->setBufferSize(512);

    _mqttClient->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onMqttMessageReceived(topic, payload, length); });
}

void MqttLogistics::onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    // A lot of the arduino string functions are really useful, so we'll use it here
    String topicStr = getIncomingTopicAsString(topic);
    String payloadStr = getIncomingPayloadAsString(payload, length);

    // Handle getInfoAllNodes request
    if (payloadStr.equalsIgnoreCase(F("getInfoAllNodes")))
    {
        _lastCommand = payloadStr.c_str();

        String messageToSend = String(SECRETS::MQTT_ID) + " is up at IPv4: " + ETH.localIP().toString();
        MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, messageToSend.c_str());
    }

    // If message isn't coming in on the commands topic, we don't care
    if (!topicStr.equalsIgnoreCase(SECRETS::TOPIC_CONTROLLER_COMMANDS))
        return;

    // Change thermostat mode
    if (payloadStr.startsWith("SET_MODE_"))
        setThermostatMode(payloadStr);

    // Change thermostat setpoint
    if (payloadStr.startsWith("SET_SETPOINT_"))
        setThermostatSetpoint(payloadStr);

}

void MqttLogistics::setThermostatSetpoint(String commandString)
{
    std::string setpointString;

    for (int i = 13; i < commandString.length(); i++)
        setpointString += commandString[i];

    float setpointFloat = std::stof(setpointString);

    _currentThermostatStatus->CurrentSetpoint = setpointFloat;
}

void MqttLogistics::setThermostatMode(const String &payloadStr)
{
    _lastCommand = payloadStr.c_str();

    if (payloadStr.endsWith("COOL"))
    {
        _currentThermostatStatus->ThermostatMode = ModeCooling;
    }
    else if (payloadStr.endsWith("MAINTAIN"))
    {
        _currentThermostatStatus->ThermostatMode = ModeMaintainingRange;
    }
    else if (payloadStr.endsWith("NORMAL_HEAT"))
    {
        _currentThermostatStatus->ThermostatMode = ModeHeating;
    }
    else if (payloadStr.endsWith("FAN_ONLY_ON"))
    {
        _currentThermostatStatus->FanMode = FanAlwaysOn;
    }
    else if (payloadStr.endsWith("FAN_AUTO_ON"))
    {
        _currentThermostatStatus->FanMode = FanOnAutomatically;
    }
    else if (payloadStr.endsWith("OFF"))
    {
        _currentThermostatStatus->ThermostatMode = ModeOff;
        _currentThermostatStatus->FanMode = FanOnAutomatically;
    }
}

void MqttLogistics::reconnectMqttIfNotConnected()
{
    // Loop until we're reconnected
    while (!MqttLogistics::_mqttClient->connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(SECRETS::MQTT_SERVER);

        // Attempt to connect
        if (MqttLogistics::_mqttClient->connect("WT32_ETH01_Thermostat", SECRETS::MQTT_USER, SECRETS::MQTT_PASSWORD))
        {
            Serial.println("...connected");

            // Once connected, publish an announcement...
            String data = "Hello from WT32_ETH01_Thermostat at IPv4: " + ETH.localIP().toString();
          MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, data.c_str());

            // ... and resubscribe
          MqttLogistics::_mqttClient->subscribe(SECRETS::TOPIC_CONTROLLER_COMMANDS);
          MqttLogistics::_mqttClient->subscribe(SECRETS::TOPIC_GET_INFO_ALL);
        }
        else
        {
            Serial.print("Attempting MQTT connection to ");
            Serial.print(SECRETS::MQTT_SERVER);

            Serial.print("...failed, rc=");
            Serial.print(MqttLogistics::_mqttClient->state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttLogistics::publish(const char* topic, const char* payload)
{
    MqttLogistics::_mqttClient->publish(topic, payload);
}

void MqttLogistics::loopClient()
{
    MqttLogistics::_mqttClient->loop();
}

std::string MqttLogistics::getLastCommand()
{
    return _lastCommand;
}

String MqttLogistics::getIncomingPayloadAsString(uint8_t *payload, unsigned int payloadLength)
{
    String payloadStr = "";

    // Convert payload to string
    for (int i = 0; i < payloadLength; i++)
    {
        payloadStr += ((char)payload[i]);
    }

    Serial.print("Payload is [");
    Serial.print(payloadStr);
    Serial.print("] ");
    Serial.println();

    return payloadStr;
}

String MqttLogistics::getIncomingTopicAsString(char *topic)
{
    String topicStr = "";

    // Convert topic to string
    for (int i = 0; i < strlen(topic); i++)
    {
        topicStr += ((char)topic[i]);
    }

    Serial.print("Topic is [");
    Serial.print(topicStr);
    Serial.print("] ");
    Serial.println();

    return topicStr;
}
