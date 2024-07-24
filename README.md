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
The original WiFi-Module is an ESP8266 and it's original Tuya firmware can be replaced with Tasmota or esphome. It's case is easy to open and solderpads for serial connection are available. The wired UART for the connection to the AC's mainboard uses tx_pin: GPIO15 / rx_pin: GPIO13

### Donation: 
- [ЮMoney](https://yoomoney.ru/fundraise/XBIABgGlKEA.230703)
