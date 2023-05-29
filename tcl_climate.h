#include "esphome.h"

class MyCustomClimate : public PollingComponent, public Climate, public UARTDevice {
 public:

  MyCustomClimate(UARTComponent *parent) : UARTDevice(parent) {}
  MyCustomClimate() : PollingComponent() {}

  uint8_t set_cmd_base[] = {0xBB, 0x00, 0x01, 0x03, 0x1D, 0x00, 0x00, 0x64, 0x03, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  union {
    struct {
      uint8_t header;
      uint8_t byte_1;
      uint8_t byte_2;
      uint8_t type;
      uint8_t len;
      uint8_t byte_5;
      uint8_t byte_6;
      
      uint8_t mode : 4;
      uint8_t power : 1;
      uint8_t disp : 1;
      uint8_t eco : 1;
      uint8_t turbo : 1;

      uint8_t temp : 4;
      uint8_t fan : 3;
      uint8_t byte_8_bit_7 : 1;

      uint8_t byte_9;

      uint8_t byte_10_bit_0_4 : 5;
      uint8_t hswing : 1;
      uint8_t vswing : 1;
      uint8_t byte_10_bit_7 : 1;

      uint8_t byte_11;
      uint8_t byte_12;
      uint8_t byte_13;
      uint8_t byte_14;
      uint8_t byte_15;
      uint8_t byte_16;
      uint8_t byte_17;
      uint8_t byte_18;
      uint8_t byte_19;

      uint8_t byte_20;
      uint8_t byte_21;
      uint8_t byte_22;
      uint8_t byte_23;
      uint8_t byte_24;
      uint8_t byte_25;
      uint8_t byte_26;
      uint8_t byte_27;
      uint8_t byte_28;
      uint8_t byte_29;

      uint8_t byte_30;
      uint8_t byte_31;
      uint8_t byte_32;
      
      uint8_t byte_33_bit_0_6 : 7;
      uint8_t mute : 1;

      uint8_t byte_34;
      uint8_t byte_35;
      uint8_t byte_36;
      uint8_t byte_37;
      uint8_t byte_38;
      uint8_t byte_39;

      uint8_t byte_40;
      uint8_t byte_41;
      uint8_t byte_42;
      uint8_t byte_43;
      uint8_t byte_44;
      uint8_t byte_45;
      uint8_t byte_46;
      uint8_t byte_47;
      uint8_t byte_48;
      uint8_t byte_49;

      uint8_t byte_50;
      uint8_t byte_51;
      uint8_t byte_52;
      uint8_t byte_53;
      uint8_t byte_54;
      uint8_t byte_55;
      uint8_t byte_56;
      uint8_t byte_57;
      uint8_t byte_58;
      uint8_t byte_59;

      uint8_t xor_sum;
    } data;
    uint8_t raw[61];
  } get_cmd_resp;

  union {
    struct {
      uint8_t header;
      uint8_t byte_1;
      uint8_t byte_2;
      uint8_t type;
      uint8_t len;
      uint8_t byte_5;
      uint8_t byte_6;
      
      uint8_t byte_7_bit_0_1 : 2;
      uint8_t power : 1;
      uint8_t off_timer_en : 1;
      uint8_t on_timer_en : 1;
      uint8_t beep : 1;
      uint8_t disp : 1;
      uint8_t eco : 1;

      uint8_t mode : 4;
      uint8_t byte_8_bit_4_5 : 2;
      uint8_t turbo : 1;
      uint8_t mute : 1;

      uint8_t temp : 4;
      uint8_t byte_9_bit_4_7 : 4;

      uint8_t fan : 3;
      uint8_t vswing : 3;
      uint8_t byte_10_bit_6 : 1;
      uint8_t byte_10_bit_7 : 1;

      uint8_t byte_11;
      uint8_t byte_12;
      uint8_t byte_13;

      uint8_t byte_14_bit_0_2 : 3;
      uint8_t hswing : 1;
      uint8_t byte_14_bit_4 : 1;
      uint8_t half_degree : 1;
      uint8_t byte_14_bit_6_7 : 2;

      uint8_t byte_15;
      uint8_t byte_16;
      uint8_t byte_17;
      uint8_t byte_18;
      uint8_t byte_19;

      uint8_t byte_20;
      uint8_t byte_21;
      uint8_t byte_22;
      uint8_t byte_23;
      uint8_t byte_24;
      uint8_t byte_25;
      uint8_t byte_26;
      uint8_t byte_27;
      uint8_t byte_28;
      uint8_t byte_29;

      uint8_t byte_30;
      uint8_t byte_31;
      uint8_t byte_32;
      uint8_t byte_33;

      uint8_t xor_sum;
    } data;
    uint8_t raw[35];
  } set_cmd;

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
      if(len == sizeof(get_cmd_resp) && buffer[3] == 0x04) {
        memcpy(get_cmd_resp.raw, buffer, len);
        print_hex_str(buffer, len);
        if (is_valid_xor(buffer, len)) {
          float curr_temp = (((buffer[17] << 8) | buffer[18]) / 374 - 32) / 1.8;

          if (get_cmd_resp.data.power == 0x00) this->mode = climate::CLIMATE_MODE_OFF;
          else if (get_cmd_resp.data.mode == 0x01) this->mode = climate::CLIMATE_MODE_COOL;
          else if (get_cmd_resp.data.mode == 0x03) this->mode = climate::CLIMATE_MODE_DRY;
          else if (get_cmd_resp.data.mode == 0x02) this->mode = climate::CLIMATE_MODE_FAN_ONLY;
          else if (get_cmd_resp.data.mode == 0x04) this->mode = climate::CLIMATE_MODE_HEAT;
          else if (get_cmd_resp.data.mode == 0x05) this->mode = climate::CLIMATE_MODE_AUTO;


          if (get_cmd_resp.data.turbo) this->custom_fan_mode = esphome::to_string("Turbo");
          else if (get_cmd_resp.data.mute) this->custom_fan_mode = esphome::to_string("Mute");
          else if (get_cmd_resp.data.fan == 0x00) this->custom_fan_mode = esphome::to_string("Auto");
          else if (get_cmd_resp.data.fan == 0x01) this->custom_fan_mode = esphome::to_string("Low");
          else if (get_cmd_resp.data.fan == 0x04) this->custom_fan_mode = esphome::to_string("Low - Medium");
          else if (get_cmd_resp.data.fan == 0x02) this->custom_fan_mode = esphome::to_string("Medium");
          else if (get_cmd_resp.data.fan == 0x05) this->custom_fan_mode = esphome::to_string("Medium - High");
          else if (get_cmd_resp.data.fan == 0x03) this->custom_fan_mode = esphome::to_string("High");


          if (get_cmd_resp.data.hswing && get_cmd_resp.data.vswing) this->swing_mode = climate::CLIMATE_SWING_BOTH;
          else if (!get_cmd_resp.data.hswing && !get_cmd_resp.data.vswing) this->swing_mode = climate::CLIMATE_SWING_OFF;
          else if (get_cmd_resp.data.vswing) this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
          else if (get_cmd_resp.data.hswing) this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;


          ESP_LOGI("TCL", "fan %02X", get_cmd_resp.data.fan);
          ESP_LOGI("TCL", "mode %02X", get_cmd_resp.data.mode);
          this->target_temperature = float(get_cmd_resp.data.temp + 16);
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