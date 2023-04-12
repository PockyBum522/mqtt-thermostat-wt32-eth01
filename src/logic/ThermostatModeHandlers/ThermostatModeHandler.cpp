#include "ThermostatModeHandler.h"
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "models/PinDefinitions.h"

ThermostatModeHandler::ThermostatModeHandler(CurrentThermostatStatus* currentThermostatStatus)
{
    ThermostatModeHandler::_currentThermostatStatus = currentThermostatStatus;
}

void ThermostatModeHandler::handleCurrentMode()
{

    unsigned long secondsSinceLastChecked = _currentThermostatStatus->CurrentSeconds - _lastModeCheckAtSeconds;

    if (secondsSinceLastChecked > _currentThermostatStatus->CurrentSettings.ModeCheckIntervalSeconds)
    {
        _lastModeCheckAtSeconds = _currentThermostatStatus->CurrentSeconds;                 // Reset time since last check

        if (_lastFanMode != _currentThermostatStatus->CurrentFanMode)
            handleFanModeChange();

        if (_lastThermostatMode != _currentThermostatStatus->CurrentThermostatMode)
        {
            handleReverserValveChange();
            handleCurrentCompressorMode();
        }

        setAllRelaysToNewStates();
    }
}

void ThermostatModeHandler::handleFanModeChange()
{
    _lastThermostatInternalMode = _currentThermostatInternalMode;

    switch (_currentThermostatStatus->CurrentFanMode)
    {
        case FanAlwaysOn:
            if (_currentThermostatStatus->CurrentThermostatMode == ThermostatOff)
                _currentThermostatInternalMode = InternalFanOnlyOn;
            break;

        case FanAutoOn:

            break;

        case FanOff:
            digitalWrite(PIN_GREEN_FAN_CALL, STATE_OFF);
            break;

        // Just here so if we add more (hmm...) in the future we'll get a warning to handle them
        case FanUninitialized:
            break;
    }
}

void ThermostatModeHandler::handleCurrentCompressorMode()
{
    switch (_currentThermostatStatus->CurrentThermostatMode)
    {
        case ThermostatCool:

            handleReverserValveChange();
            turnOnCompressorWithTimeout();
            break;

        case ThermostatHeat:
            turnAllThermostatRelaysOff();

            turnOnCompressorWithTimeout();
            break;

        case ThermostatMaintainRange:
            turnAllThermostatRelaysOff();

            break;

        case ThermostatEmergencyHeat:
            turnAllThermostatRelaysOff();

            break;

        case ThermostatOff:
            turnAllThermostatRelaysOff();

            break;

        // Just here so if we add more in the future we'll get a warning to handle them
        case ErrorSensorTemperatureProblem:
        case ThermostatUninitialized:
            break;
    }
}

void ThermostatModeHandler::handleReverserValveChange()
{
    switch (_currentThermostatStatus->CurrentThermostatMode)
    {
        case ThermostatCool:
            digitalWrite(PIN_ORANGE_REVERSER_VALVE, STATE_ON);
            break;

        case ThermostatMaintainRange:
            // This will be interesting

            break;

        // Heat is all reverser valve off
        case ThermostatHeat:
        case ThermostatEmergencyHeat:
        case ThermostatOff:
            digitalWrite(PIN_ORANGE_REVERSER_VALVE, STATE_OFF);
            break;

        // Just here so if we add more in the future we'll get a warning to handle them
        case ThermostatUninitialized:
        case ErrorSensorTemperatureProblem:
            break;
    }
}

void ThermostatModeHandler::turnAllThermostatRelaysOff()
{
    // Get compressor status before we change it
    bool currentCompressorStatus = !digitalRead(PIN_YELLOW_COMPRESSOR_CALL); // Invert cause ESP32 logic is inverted

    // If we were on, and are now turning off, update the LastCompressorOffAt
    if (currentCompressorStatus)
        _currentThermostatStatus->LastCompressorOffAtSeconds = _currentThermostatStatus->CurrentSeconds;

    digitalWrite(PIN_YELLOW_COMPRESSOR_CALL, STATE_OFF);

    digitalWrite(PIN_WHITE_AUX_HEAT, STATE_OFF);

    digitalWrite(PIN_ORANGE_REVERSER_VALVE, STATE_OFF);
}

void ThermostatModeHandler::turnOnCompressorWithTimeout()
{
    // Give them sane names
    unsigned long lastCompressorOffAtSeconds = _currentThermostatStatus->LastCompressorOffAtSeconds;
    unsigned long currentSeconds = _currentThermostatStatus->CurrentSeconds;
    unsigned long compressorTimeoutSeconds = _currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;

    // check timeout
    if (lastCompressorOffAtSeconds > (currentSeconds + compressorTimeoutSeconds))
        digitalWrite(PIN_YELLOW_COMPRESSOR_CALL, STATE_ON);
}

void ThermostatModeHandler::setAllRelaysToNewStates()
{

}
