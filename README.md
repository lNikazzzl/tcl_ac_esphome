# Integration of TCL-based air conditioners for Home Assistant

### Implemented:
- Split system modes (auto, cool, dry, fan only, heat)
- Fan modes (mute, min, min-mid, mid, mid-high, high, turbo)
- Indoor unit temperature
- Target temperature
- Swing mode in additional settings

### Tested on:
- Royal Clima rci-pf40hn
- Lennox LI036CI-180P432
- SunWind SW-18
- Kesser Split 12000/BTU
- Veska VSK-12000BTU (likely TCL TAC-12chsa/xa73i, as the Kesser above)
- TCL AC model: ELI09 INV/R1
- Royal Thermo Barocco RTB/in-09HN1
- Hantech AC (some model??)
- Daizuki DXTH12E426-20
- Royal Clima RCI-AR22
- StarLight 12000 BTU
- Danby DAS180EAQHWDB
- Royal Clima RCI-GLF07HN
- Royal Clima 12HN8
- Della mini-split (TCL rebrand; ESP8266 D1 mini, UART 9600 8E1)

### Disable the serial logger
On ESP8266 the AC is connected to UART0 (GPIO1/GPIO3), which is the same UART the ESPHome logger writes to by default. Log output on that UART gets mixed into the AC link and corrupts commands (in testing about half of the commands were dropped). Turn the serial logger off; logs still work over Wi-Fi / the API:
```
logger:
  baud_rate: 0
```

### Many users recommend using firmware based on a different UART, for example:
If you are having trouble where the air conditioner turns on but does not respond to other commands, use this.
```
uart:
  id: uart_bus
  tx_pin: 17
  rx_pin: 16
  baud_rate: 9600
  parity: EVEN
```

### Tuya Module 32001-000140
The [original WiFi-Module](https://github.com/user-attachments/assets/f1888a35-ba68-4869-9790-71ff8c572931) is an ESP8266 and it's original Tuya firmware can be replaced with Tasmota or esphome. It's case is easy to open and [solderpads for serial connection](https://github.com/user-attachments/assets/4515421f-4346-4248-aba7-d4db3886ac40) are available.
The wired UART for the connection to the AC's mainboard uses tx_pin: GPIO15 / rx_pin: GPIO13

### Donation: 
- kaspi kz (outside Russia) 4400430344051161
- sber (Russia) 2202205034977568
