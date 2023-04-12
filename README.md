# mqtt-thermostat-wt32-eth01
Making my own thermostat from scratch with a ESP32

# Wiring:

## SHT30:

| SHT30 PIN | Wire Color | ESP8266 Pin | ESP32 Pin |
|-----------|------------|-------------|-----------|
| VIN       | Red        | 3.3v        | 3.3v      |
| SCL       | Yellow     | D1          | IO 22     |
| SDA       | White      | D2          | IO 21     |    
| Ground    | Green      | Ground      | Ground    |    

## Relays:

| Relay Preprocessor Directive Name | ESP32 GPIO | WT32_ETH01 Pin Silkscreened Label | HVAC Function/Preprocessor Directive |
|-----------------------------------|------------|-----------------------------------|--------------------------------------|
| PIN_RELAY_01                      | GPIO 2     | IO2                               | PIN_GREEN_FAN_CALL                   |
| PIN_RELAY_02                      | GPIO 4     | IO4                               | PIN_YELLOW_COMPRESSOR_CALL           |
| PIN_RELAY_03                      | GPIO 33    | 485_EN                            | PIN_ORANGE_REVERSER_VALVE            |
| PIN_RELAY_04                      | GPIO 14    | IO14                              | PIN_WHITE_AUX_HEAT                   |
| PIN_RELAY_05                      | GPIO 15    | IO15                              |                                      |
| PIN_RELAY_06                      | GPIO 5     | RXD                               |                                      |
