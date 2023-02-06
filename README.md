# esphome-junctek_kgf
Component for esphome to read status from a Junctek KG-F coulometer/battery monitor via UART

## Features
Connects to the Junctek KGF series battery monitor via UART (RS-485 adapter needed) and retrieves the following values:
* Battery Voltage
* Battery Percent
* Current Amps
* Temperature

## Requirements
* ESPHome

## Tested setup
Tested on ESP32 using a RS-485 uart into a Junctek KG110F, but should work on an ESP8266 and any of the KG-F series

## Usage
### Connect hardware.
The ESP32 needs to be connected via an RS-485 module to the RS-485 on the monitor using a 4cp4 connector.

## ESPHOME Config
The applicable config for the device should look something like:

```yaml
external_components:
  - source: github://tfyoung/esphome-junctek_kgf

uart:
  tx_pin: 26
  rx_pin: 27
  baud_rate: 115200

sensor:
  - platform: junctek_kgf
    address: 1
    invert_current: true
    voltage:
      name: "Battery Voltage"
    current:
      name: "Battery Current"
    battery_level:
      name: "Battery Level"
    temperature:
      name: "Battery Temperature"
```

Not all sensors need to be added.
Address is assumed to be 1 if not provided. (this is configured on the monitor)
invert_current: This inverts the reported current, it's recommended to include this option with either true or false (which ever makes the current make more sense for your setup). The default is currently false (and false will match previous behaviour), but may change to true in future updates.
## Future work
More sensors/statistics are possible, as is adjusting various configuration, but haven't currently been added. File an issue if there anything you want to see.
