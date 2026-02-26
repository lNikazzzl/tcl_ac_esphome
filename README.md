# Integration of TCL-based air conditioners for Home Assistant

### Implemented:
- Split system modes (auto, cool, dry, fan only, heat)
- Fan modes (mute, min, min-mid, mid, mid-high, high, turbo)
- Indoor unit temperature
- Target temperature
- Swing mode (only h/v, and visualization only)

### Tested on:
- Royal Clima rci-pf40hn
- Lennox LI036CI-180P432
- SunWind SW-18
- Kesser Split 12000/BTU
- Veska VSK-12000BTU (likely TCL TAC-12chsa/xa73i, as the Kesser above)

### Unsuccessfully tested:
- TCL-12chsa/tpg

### Tuya Module 32001-000140
The [original WiFi-Module](https://github.com/user-attachments/assets/f1888a35-ba68-4869-9790-71ff8c572931) is an ESP8266 and it's original Tuya firmware can be replaced with Tasmota or esphome. It's case is easy to open and [solderpads for serial connection](https://github.com/user-attachments/assets/4515421f-4346-4248-aba7-d4db3886ac40) are available.
The wired UART for the connection to the AC's mainboard uses tx_pin: GPIO15 / rx_pin: GPIO13

### Donation: 
- kaspi kz (outside Russia) 4400430344051161
- sber (Russia) 2202205034977568
