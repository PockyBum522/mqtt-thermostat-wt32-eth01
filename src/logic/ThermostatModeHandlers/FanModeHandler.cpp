#include "FanModeHandler.h"

FanModeHandler::FanModeHandler(CurrentThermostatStatus *currentThermostatStatus)
{
    _currentThermostatStatus = currentThermostatStatus;
}

bool FanModeHandler::getNextFanState()
{
    float setpoint = _currentThermostatStatus->CurrentSetpoint;
    float marginFahrenheit = _currentThermostatStatus->CurrentSettings.MarginFahrenheit;

    float highLimit = setpoint + marginFahrenheit;
    float lowLimit = setpoint - marginFahrenheit;

    float currentFahrenheit = _currentThermostatStatus->CurrentTemperatureFahrenheit;

    // Handle cases where fan is just on
    if (_currentThermostatStatus->CurrentFanMode == FanAlwaysOn)
        return true;

    // Handle special cases
    switch (_currentThermostatStatus->CurrentThermostatMode)
    {
        case ThermostatCool:
            if (currentFahrenheit > highLimit)
            {
                return true;
            }


        case ThermostatHeat:
            break;

        case ThermostatMaintainRange:
            break;

        case ThermostatEmergencyHeat:
            break;

        case ThermostatOff:
            break;

        case ThermostatUninitialized:
        case ErrorSensorTemperatureProblem:
            break;
    }

    if (currentFahrenheit > highLimit ||
        currentFahrenheit < lowLimit)

}

bool FanModeHandler::setFanStateBasedOnSystemStatus()
{

}

void FanModeHandler::turnFanOn()
{
    _lastFanState = _currentFanState;
    _currentFanState = STATE_ON;

    digitalWrite(PIN_GREEN_FAN_CALL, _currentFanState);
}

void FanModeHandler::turnFanOff()
{
    _lastFanState = _currentFanState;
    _currentFanState = STATE_OFF;

    digitalWrite(PIN_GREEN_FAN_CALL, _currentFanState);
}
