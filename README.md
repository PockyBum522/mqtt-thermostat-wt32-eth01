# mqtt-thermostat-wt32-eth01
Making my own thermostat from scratch with a ESP32 to drive my heat-pump based central air

Parts:
* WT32-ETH01 ESP32/Ethernet combination board
* 8 channel relay board
* SHT30 I2c Temperature Sensor

# Status/Features

* Basic control and reporting to and from home assistant over MQTT - Working!
* Cooling - Working!
* Heating - Working!
* ElegantOTA for updates over ethernet - Working!
* Restore preset settings shortly after startup in case of brownout - Working!
* Automatic mode to stay in a set range with both heating and cooling - Not yet.

# Wiring

## SHT30

| SHT30 PIN | Wire Color | ESP8266 Pin | ESP32 Pin | WT32-ETH01 Pin | WT32-ETH01 Pin Screen-printed Label | 4-pin Header Pin Number |
|-----------|------------|-------------|-----------|----------------|-------------------------------------|-------------------------|
| Ground    | Black      | Ground      | Ground    | GND            | GND                                 | 01                      |
| VIN       | Red        | 3.3v        | 3.3v      | 3v3            | 3V3                                 | 02                      |
| SCL       | Yellow     | D1          | IO 22     | IO 17          | TXD                                 | 03                      |
| SDA       | White      | D2          | IO 21     | IO 32          | CFG                                 | 04                      |

## Relays

| Relay Preprocessor Directive Name | ESP32 GPIO | WT32_ETH01 Pin Silkscreen Label | HVAC Function/Preprocessor Directive |
|-----------------------------------|------------|---------------------------------|--------------------------------------|
| PIN_RELAY_01                      | GPIO 2     | IO2                             | PIN_GREEN_FAN_CALL                   |
| PIN_RELAY_02                      | GPIO 4     | IO4                             | PIN_YELLOW_COMPRESSOR_CALL           |
| PIN_RELAY_03                      | GPIO 33    | 485_EN                          | PIN_ORANGE_REVERSER_VALVE            |
| PIN_RELAY_04                      | GPIO 14    | IO14                            | PIN_WHITE_AUX_HEAT                   |
| PIN_RELAY_05                      | GPIO 15    | IO15                            |                                      |
| PIN_RELAY_06                      | GPIO 5     | RXD                             |                                      |
