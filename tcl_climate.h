#include "esphome.h"

class MyCustomClimate : public PollingComponent, public Climate, public UARTDevice {
 public:

  MyCustomClimate(UARTComponent *parent) : UARTDevice(parent) {}
  MyCustomClimate() : PollingComponent() {}

  uint8_t turbo;
  uint8_t eco;
  uint8_t disp;
  uint8_t power;
  uint8_t ac_mode;
  uint8_t temp;
  uint8_t fan;
  uint8_t h_swing;
  uint8_t v_swing;
  uint8_t sleep;
  uint8_t mute;


  void setup() override {
    // This will be called by App.setup()
    set_update_interval(1000);
  }
  
  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      ClimateMode climate_mode = *call.get_mode();
      // Send mode to hardware

      uint8_t set_cmd[] = {0xBB, 0x00, 0x01, 0x03, 0x1D, 0x00, 0x00, 0x64, 0x03, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      uint8_t _eco = 0x00; // ?
      uint8_t _disp = 0x01;
      uint8_t _beep = 0x01;
      uint8_t _ontimeren = 0x00;
      uint8_t _offtimeren = 0x00;
      uint8_t _power = 0x00;
      uint8_t _mute = mute ? 0x01 : 0x00;
      uint8_t _turbo = turbo ? 0x01 : 0x00;
      uint8_t _health = 0x00;
      uint8_t _mode = 0;



      switch (ac_mode) {
        case 0x01:
          _mode = 0x03;
          break;
        case 0x03:
          _mode = 0x02;
          break;
        case 0x02:
          _mode = 0x07;
          break;
        case 0x04:
          _mode = 0x01;
          break;
        case 0x05:
          _mode = 0x08;
          break;
      }


      if (climate_mode == climate::CLIMATE_MODE_OFF) {
        _power = 0x00;
      } else {
        _power = 0x01;
        switch (climate_mode) {
          case climate::CLIMATE_MODE_COOL:
            _mode = 0x03;
            break;
          case climate::CLIMATE_MODE_DRY:
            _mode = 0x02;
            break;
          case climate::CLIMATE_MODE_FAN_ONLY:
            _mode = 0x07;
            break;
          case climate::CLIMATE_MODE_HEAT:
            _mode = 0x01;
            break;
          case climate::CLIMATE_MODE_AUTO:
            _mode = 0x08;
            break;
        }
      }
      uint8_t _swingv = 0;
      uint8_t _fan = fan >> 4;
      uint8_t _halfdegree = 0x00;
      uint8_t _swingh = 0x00;

      set_cmd[7] = _eco << 7 | _disp << 6 | _beep << 5 | _ontimeren << 4 | _offtimeren << 3 | _power << 2;
      set_cmd[8] = _mute << 7 | _turbo << 6 | _mode;
      set_cmd[9] = 31 - temp;
      set_cmd[10] = _swingv << 3 | _fan;
      set_cmd[14] = _halfdegree << 5 | _swingh << 3;

      for (int i = 0; i < sizeof(set_cmd) - 1; i++) set_cmd[sizeof(set_cmd) - 1] ^= set_cmd[i];
      write_array(set_cmd, sizeof(set_cmd));

      // Publish updated state
    //  this->mode = mode;
     // this->publish_state();
    }
    if (call.get_target_temperature().has_value()) {
      // User requested target temperature change
      float temp = *call.get_target_temperature();
      // Send target temp to climate
      // ...
    }
  }
  ClimateTraits traits() override {
    // The capabilities of the climate device
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_FAN_ONLY, climate::CLIMATE_MODE_DRY, climate::CLIMATE_MODE_AUTO});
    traits.set_supported_custom_fan_modes({"Turbo", "Mute", "Auto", "Low", "Low - Medium", "Medium", "Medium - High", "High"});
    traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_BOTH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL});
    traits.set_visual_min_temperature(16.0);
    traits.set_visual_max_temperature(31.0);
    traits.set_visual_target_temperature_step(1.0);
    return traits;
  }

  void update() override {
    // This will be called every "update_interval" milliseconds.
    uint8_t req_cmd[] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};
    write_array(req_cmd, sizeof(req_cmd));
  }

  int read_data_line(int readch, uint8_t *buffer, int len)
  {
    static int pos = 0;
    static bool wait_len = false;
    static int skipch = 0;

    //ESP_LOGI("custom", "%02X", readch);

    if (readch >= 0) {
      if (readch == 0xBB && skipch == 0 && !wait_len) {
        pos = 0;
        skipch = 3; // wait char with len
        wait_len = true;
        if (pos < len-1) buffer[pos++] = readch;
      } else if (skipch == 0 && wait_len) {
        if (pos < len-1) buffer[pos++] = readch;
        skipch = readch + 1; // +1 control sum
        ESP_LOGI("TCL", "len: %d", readch);
        wait_len = false;
      } else if (skipch > 0) {
        if (pos < len-1) buffer[pos++] = readch;
        if (--skipch == 0 && !wait_len) return pos;
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  bool is_valid_xor(uint8_t *buffer, int len)
  {
    uint8_t xor_byte = 0;
    for (int i = 0; i < len - 1; i++) xor_byte ^= buffer[i];
    if (xor_byte == buffer[len - 1]) return true;
    else {
      ESP_LOGW("TCL", "No valid xor crc %02X (calculated %02X)", buffer[len], xor_byte);
      return false;
    }
    
  }

  void print_hex_str(uint8_t *buffer, int len)
  {
    char str[250] = {0};
    char *pstr = str;
    if (len * 2 > sizeof(str)) ESP_LOGE("TCL", "too long byte data");

    for (int i = 0; i < len; i++) {
      pstr += sprintf(pstr, "%02X ", buffer[i]);
    }

    ESP_LOGI("TCL", "%s", str);
  }

  void loop() override {
    const int max_line_length = 100;
    static uint8_t buffer[max_line_length];
    
    while (available()) {
      int len = read_data_line(read(), buffer, max_line_length);
      if(len > 0) {
        print_hex_str(buffer, len);
        if (is_valid_xor(buffer, len)) {

          turbo = buffer[7] & 0x80;
          eco = buffer[7] &  0x40;
          disp = buffer[7] & 0x20;
          power = buffer[7] & 0x10;
          ac_mode = buffer[7] & 0x0F;
          temp = (buffer[8] & 0x07) + 16;
          fan = buffer[8] & 0x070;
          h_swing = buffer[10] & 0x20;
          v_swing = buffer[10] & 0x40;
          sleep = buffer[19] & 0x01;
          mute = buffer[33] & 0x80;

          float curr_temp = (((buffer[17] << 8) | buffer[18]) / 374 - 32) / 1.8;

          if (power == 0x00) this->mode = climate::CLIMATE_MODE_OFF;
          else if (ac_mode == 0x01) this->mode = climate::CLIMATE_MODE_COOL;
          else if (ac_mode == 0x03) this->mode = climate::CLIMATE_MODE_DRY;
          else if (ac_mode == 0x02) this->mode = climate::CLIMATE_MODE_FAN_ONLY;
          else if (ac_mode == 0x04) this->mode = climate::CLIMATE_MODE_HEAT;
          else if (ac_mode == 0x05) this->mode = climate::CLIMATE_MODE_AUTO;


          if (turbo == 0x80) this->custom_fan_mode = esphome::to_string("Turbo");
          else if (mute == 0x80) this->custom_fan_mode = esphome::to_string("Mute");
          else if (fan == 0x00) this->custom_fan_mode = esphome::to_string("Auto");
          else if (fan == 0x10) this->custom_fan_mode = esphome::to_string("Low");
          else if (fan == 0x40) this->custom_fan_mode = esphome::to_string("Low - Medium");
          else if (fan == 0x20) this->custom_fan_mode = esphome::to_string("Medium");
          else if (fan == 0x50) this->custom_fan_mode = esphome::to_string("Medium - High");
          else if (fan == 0x30) this->custom_fan_mode = esphome::to_string("High");


          if (h_swing == 0x20 && v_swing == 0x40) this->swing_mode = climate::CLIMATE_SWING_BOTH;
          else if (h_swing == 0x00 && v_swing == 0x00) this->swing_mode = climate::CLIMATE_SWING_OFF;
          else if (v_swing == 0x40) this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
          else if (h_swing == 0x20) this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;


          ESP_LOGI("TCL", "fan %02X", fan);
          this->target_temperature = float(temp);
          this->current_temperature = curr_temp;
          this->publish_state();
        }
        //publish_state(buffer);
      }
    }
    
    //this->target_temperature = 20.0;
    //this->publish_state();
      // if(readline(read(), buffer, max_line_length) > 0) {
      //   publish_state(buffer);
      // }
   // }
  }
};