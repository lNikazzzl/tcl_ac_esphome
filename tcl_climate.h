#include "esphome.h"

class MyCustomClimate : public PollingComponent, public Climate, public UARTDevice {
public:

  MyCustomClimate(UARTComponent *parent) : UARTDevice(parent) {}
  MyCustomClimate() : PollingComponent() {}
  bool is_changed : 1;

  union get_cmd_resp_t{
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
  };

  union set_cmd_t {
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

      uint8_t byte_11_bit_0_2 : 3;
      uint8_t hswing : 1;
      uint8_t byte_11_bit_4_7 : 4;

      uint8_t byte_12;
      uint8_t byte_13;

      uint8_t byte_14_bit_0_2 : 3;
      uint8_t byte_14_bit_3 : 1;
      uint8_t byte_14_bit_4 : 1;//uint8_t hswing : 1;
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
  };

  bool ready_to_send_set_cmd_flag = false;

  uint8_t set_cmd_base[35] = {0xBB, 0x00, 0x01, 0x03, 0x1D, 0x00, 0x00, 0x64, 0x03, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  get_cmd_resp_t m_get_cmd_resp = {0};
  set_cmd_t m_set_cmd = {0};

  void set_current_temperature (float current_temperature) {
    if (this->current_temperature == current_temperature) return;
    this->is_changed = true;
    this->current_temperature = current_temperature;
  }

  void set_custom_fan_mode  (const std::string &fan_mode) {
    if (this->custom_fan_mode == fan_mode) return;
    this->is_changed = true;
    this->custom_fan_mode = fan_mode;
  }

  void set_mode  (esphome::climate::ClimateMode mode) {
    if (this->mode == mode) return;
    this->is_changed = true;
    this->mode = mode;
  }

  void set_swing_mode (esphome::climate::ClimateSwingMode swing_mode) {
    if (this->swing_mode == swing_mode) return;
    this->is_changed = true;
    this->swing_mode = swing_mode;
  }

  void set_target_temperature (float target_temperature) {
    if (this->target_temperature == target_temperature) return;
    this->is_changed = true;
    this->target_temperature = target_temperature;
  }

  void build_set_cmd(get_cmd_resp_t * get_cmd_resp) {
    memcpy(m_set_cmd.raw, set_cmd_base, sizeof(m_set_cmd.raw));

    m_set_cmd.data.power = get_cmd_resp->data.power;
    m_set_cmd.data.off_timer_en = 0;
    m_set_cmd.data.on_timer_en = 0;
    m_set_cmd.data.beep = 1;
    m_set_cmd.data.disp = 1;
    m_set_cmd.data.eco = 0;

    switch (get_cmd_resp->data.mode) {
      case 0x01:
        m_set_cmd.data.mode = 0x03;
        break;
      case 0x03:
        m_set_cmd.data.mode = 0x02;
        break;
      case 0x02:
        m_set_cmd.data.mode = 0x07;
        break;
      case 0x04:
        m_set_cmd.data.mode = 0x01;
        break;
      case 0x05:
        m_set_cmd.data.mode = 0x08;
        break;
    }

    m_set_cmd.data.turbo = get_cmd_resp->data.turbo;
    m_set_cmd.data.mute = get_cmd_resp->data.mute;
    m_set_cmd.data.temp = 15 - get_cmd_resp->data.temp;

    switch (get_cmd_resp->data.fan) {
      case 0x00:
        m_set_cmd.data.fan = 0x00;
        break;
      case 0x01:
        m_set_cmd.data.fan = 0x02;
        break;
      case 0x04:
        m_set_cmd.data.fan = 0x06;
        break;
      case 0x02:
        m_set_cmd.data.fan = 0x03;
        break;
      case 0x05:
        m_set_cmd.data.fan = 0x07;
        break;
      case 0x03:
        m_set_cmd.data.fan = 0x05;
        break;
    }

    m_set_cmd.data.vswing = get_cmd_resp->data.vswing ? 0x07 : 0x00;
    m_set_cmd.data.hswing = get_cmd_resp->data.hswing;

    m_set_cmd.data.half_degree = 0;

    for (int i = 0; i < sizeof(m_set_cmd.raw) - 1; i++) m_set_cmd.raw[sizeof(m_set_cmd.raw) - 1] ^= m_set_cmd.raw[i];
  }

  void setup() override {
    // This will be called by App.setup()
    set_update_interval(500);
  }
  
  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      ClimateMode climate_mode = *call.get_mode();
      // Send mode to hardware

      get_cmd_resp_t get_cmd_resp = {0};
      memcpy(get_cmd_resp.raw, m_get_cmd_resp.raw, sizeof(get_cmd_resp.raw));

      if (climate_mode == climate::CLIMATE_MODE_OFF) {
        get_cmd_resp.data.power = 0x00;
      } else {
        get_cmd_resp.data.power = 0x01;
        switch (climate_mode) {
          case climate::CLIMATE_MODE_COOL:
            get_cmd_resp.data.mode = 0x01;
            break;
          case climate::CLIMATE_MODE_DRY:
            get_cmd_resp.data.mode = 0x03;
            break;
          case climate::CLIMATE_MODE_FAN_ONLY:
            get_cmd_resp.data.mode = 0x02;
            break;
          case climate::CLIMATE_MODE_HEAT:
          case climate::CLIMATE_MODE_HEAT_COOL:
            get_cmd_resp.data.mode = 0x04;
            break;
          case climate::CLIMATE_MODE_AUTO:
            get_cmd_resp.data.mode = 0x05;
            break;
          case CLIMATE_MODE_OFF:
            get_cmd_resp.data.power = 0x00;
            break;
        }
      }

      build_set_cmd(&get_cmd_resp);
      ready_to_send_set_cmd_flag = true;

      // Publish updated state
    //  this->mode = mode;
     // this->publish_state();
    }
    if (call.get_target_temperature().has_value()) {
      // User requested target temperature change
      float temp = *call.get_target_temperature();
      
      get_cmd_resp_t get_cmd_resp = {0};
      memcpy(get_cmd_resp.raw, m_get_cmd_resp.raw, sizeof(get_cmd_resp.raw));

      get_cmd_resp.data.temp = uint8_t(temp) - 16;

      build_set_cmd(&get_cmd_resp);
      ready_to_send_set_cmd_flag = true;
    }
    if (call.get_swing_mode().has_value()) {
      // User requested target temperature change
      ClimateSwingMode swing_mode = *call.get_swing_mode();

      get_cmd_resp_t get_cmd_resp = {0};
      memcpy(get_cmd_resp.raw, m_get_cmd_resp.raw, sizeof(get_cmd_resp.raw));

      switch(swing_mode) {
        case climate::CLIMATE_SWING_OFF:
          get_cmd_resp.data.hswing = 0;
          get_cmd_resp.data.vswing = 0;
          break;
        case climate::CLIMATE_SWING_BOTH:
          get_cmd_resp.data.hswing = 1;
          get_cmd_resp.data.vswing = 1;
          break;
        case climate::CLIMATE_SWING_VERTICAL:
          get_cmd_resp.data.hswing = 0;
          get_cmd_resp.data.vswing = 1;
          break;
        case climate::CLIMATE_SWING_HORIZONTAL:
          get_cmd_resp.data.hswing = 1;
          get_cmd_resp.data.vswing = 0;
          break;
      }

      build_set_cmd(&get_cmd_resp);
      ready_to_send_set_cmd_flag = true;
     
    }
    if (call.get_custom_fan_mode().has_value()) {
      ESP_LOGI("ads", "ajskndjanjanwnjwa");
      // User requested target temperature change
      std::string fan_mode = *call.get_custom_fan_mode();

      get_cmd_resp_t get_cmd_resp = {0};
      memcpy(get_cmd_resp.raw, m_get_cmd_resp.raw, sizeof(get_cmd_resp.raw));

      get_cmd_resp.data.turbo = 0x00;
      get_cmd_resp.data.mute = 0x00;
      if (fan_mode == esphome::to_string("Turbo")) { 
        get_cmd_resp.data.fan = 0x03;
        get_cmd_resp.data.turbo = 0x01;
      } else if (fan_mode == esphome::to_string("Mute")) {
        get_cmd_resp.data.fan = 0x01;
        get_cmd_resp.data.mute = 0x01;
      } else if (fan_mode == esphome::to_string("Automatic")) get_cmd_resp.data.fan = 0x00;
      else if (fan_mode == esphome::to_string("1")) get_cmd_resp.data.fan = 0x01;
      else if (fan_mode == esphome::to_string("2")) get_cmd_resp.data.fan = 0x04;
      else if (fan_mode == esphome::to_string("3")) get_cmd_resp.data.fan = 0x02;
      else if (fan_mode == esphome::to_string("4")) get_cmd_resp.data.fan = 0x05;
      else if (fan_mode == esphome::to_string("5")) get_cmd_resp.data.fan = 0x03;

      build_set_cmd(&get_cmd_resp);
      ready_to_send_set_cmd_flag = true;
     
    }
  }

  ClimateTraits traits() override {
    // The capabilities of the climate device
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_COOL, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_FAN_ONLY, climate::CLIMATE_MODE_DRY, climate::CLIMATE_MODE_AUTO});
    traits.set_supported_custom_fan_modes({"Turbo", "Mute", "Automatic", "1", "2", "3", "4", "5"});
    traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_BOTH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL});
    traits.set_visual_min_temperature(16.0);
    traits.set_visual_max_temperature(31.0);
    traits.set_visual_target_temperature_step(1.0);
    return traits;
  }

  void update() override {
    // This will be called every "update_interval" milliseconds.
    uint8_t req_cmd[] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};

    if (ready_to_send_set_cmd_flag) {
        ready_to_send_set_cmd_flag = false;
        write_array(m_set_cmd.raw, sizeof(m_set_cmd.raw));
    }
    else write_array(req_cmd, sizeof(req_cmd));
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
      if(len == sizeof(m_get_cmd_resp) && buffer[3] == 0x04) {
        memcpy(m_get_cmd_resp.raw, buffer, len);
        print_hex_str(buffer, len);
        if (is_valid_xor(buffer, len)) {
          float curr_temp = (((buffer[17] << 8) | buffer[18]) / 374 - 32) / 1.8;
          this->is_changed = false;

          if (m_get_cmd_resp.data.power == 0x00) this->set_mode(climate::CLIMATE_MODE_OFF);
          else if (m_get_cmd_resp.data.mode == 0x01) this->set_mode(climate::CLIMATE_MODE_COOL);
          else if (m_get_cmd_resp.data.mode == 0x03) this->set_mode(climate::CLIMATE_MODE_DRY);
          else if (m_get_cmd_resp.data.mode == 0x02) this->set_mode(climate::CLIMATE_MODE_FAN_ONLY);
          else if (m_get_cmd_resp.data.mode == 0x04) this->set_mode(climate::CLIMATE_MODE_HEAT);
          else if (m_get_cmd_resp.data.mode == 0x05) this->set_mode(climate::CLIMATE_MODE_AUTO);


          if (m_get_cmd_resp.data.turbo) this->set_custom_fan_mode(esphome::to_string("Turbo"));
          else if (m_get_cmd_resp.data.mute) this->set_custom_fan_mode(esphome::to_string("Mute"));
          else if (m_get_cmd_resp.data.fan == 0x00) this->set_custom_fan_mode(esphome::to_string("Automatic"));
          else if (m_get_cmd_resp.data.fan == 0x01) this->set_custom_fan_mode(esphome::to_string("1"));
          else if (m_get_cmd_resp.data.fan == 0x04) this->set_custom_fan_mode(esphome::to_string("2"));
          else if (m_get_cmd_resp.data.fan == 0x02) this->set_custom_fan_mode(esphome::to_string("3"));
          else if (m_get_cmd_resp.data.fan == 0x05) this->set_custom_fan_mode(esphome::to_string("4"));
          else if (m_get_cmd_resp.data.fan == 0x03) this->set_custom_fan_mode(esphome::to_string("5"));


          if (m_get_cmd_resp.data.hswing && m_get_cmd_resp.data.vswing) this->set_swing_mode(climate::CLIMATE_SWING_BOTH);
          else if (!m_get_cmd_resp.data.hswing && !m_get_cmd_resp.data.vswing) this->set_swing_mode(climate::CLIMATE_SWING_OFF);
          else if (m_get_cmd_resp.data.vswing) this->set_swing_mode(climate::CLIMATE_SWING_VERTICAL);
          else if (m_get_cmd_resp.data.hswing) this->set_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);


          ESP_LOGI("TCL", "fan %02X", m_get_cmd_resp.data.fan);
          ESP_LOGI("TCL", "mode %02X", m_get_cmd_resp.data.mode);
          this->set_target_temperature(float(m_get_cmd_resp.data.temp + 16));
          this->set_current_temperature(curr_temp);
          if (this->is_changed)
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
