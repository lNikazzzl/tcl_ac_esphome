//#ifdef USE_ARDUINO

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "tcl_climate.h"
#include <map>  // Add this include

namespace esphome {
namespace tcl_climate {

// Use constexpr for compile-time constants
static constexpr uint8_t REQ_CMD[] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};
static constexpr int MAX_LINE_LENGTH = 100;
static constexpr int UPDATE_INTERVAL_MS = 450;

void TCLClimate::set_current_temperature(float current_temperature) {
  if (std::abs(this->current_temperature - current_temperature) < 0.1f) return;

  // Log current temperature change
  ESP_LOGD("TCL", "Current temperature updated to: %.1f°C", current_temperature);

  this->is_changed = true;
  this->current_temperature = current_temperature;
}

void TCLClimate::set_custom_fan_mode(StringRef fan_mode) {
  StringRef current(this->get_custom_fan_mode());
  if (!current.empty() && fan_mode == current.c_str()) {
    return;
  }

  // Log the fan mode change
  ESP_LOGI("TCL", "Fan mode changed to: %s", fan_mode.c_str());
  this->is_changed = true;

  this->set_custom_fan_mode_(fan_mode.c_str());
}

void TCLClimate::set_mode(climate::ClimateMode mode) {
  if (this->mode == mode) return;
  // Log the mode change
  const char* mode_str = "";
  switch (mode) {
    case climate::CLIMATE_MODE_OFF: mode_str = "OFF"; break;
    case climate::CLIMATE_MODE_COOL: mode_str = "COOL"; break;
    case climate::CLIMATE_MODE_HEAT: mode_str = "HEAT"; break;
    case climate::CLIMATE_MODE_FAN_ONLY: mode_str = "FAN ONLY"; break;
    case climate::CLIMATE_MODE_DRY: mode_str = "DRY"; break;
    case climate::CLIMATE_MODE_AUTO: mode_str = "AUTO"; break;
    default: mode_str = "UNKNOWN"; break;
  }
  ESP_LOGI("TCL", "Climate mode changed to: %s", mode_str);
  this->is_changed = true;
  this->mode = mode;
}

void TCLClimate::set_swing_mode(climate::ClimateSwingMode swing_mode) {
  if (this->swing_mode == swing_mode) return;

  const char* swing_str = "";
  switch (swing_mode) {
    case climate::CLIMATE_SWING_OFF: swing_str = "OFF"; break;
    case climate::CLIMATE_SWING_BOTH: swing_str = "BOTH"; break;
    case climate::CLIMATE_SWING_VERTICAL: swing_str = "VERTICAL"; break;
    case climate::CLIMATE_SWING_HORIZONTAL: swing_str = "HORIZONTAL"; break;
    default: swing_str = "UNKNOWN"; break;
  }
  ESP_LOGI("TCL", "Swing mode changed to: %s", swing_str);
  this->is_changed = true;
  this->swing_mode = swing_mode;
}

void TCLClimate::set_hswing_pos(const std::string &hswing_pos) {
  if (this->hswing_pos == hswing_pos) return;
  ESP_LOGI("TCL", "Horizontal swing position: %s", hswing_pos.c_str());
  this->hswing_pos = hswing_pos;
}

void TCLClimate::set_vswing_pos(const std::string &vswing_pos) {
  if (this->vswing_pos == vswing_pos) return;

  ESP_LOGI("TCL", "Vertical swing position: %s", vswing_pos.c_str());
  this->vswing_pos = vswing_pos;
}

void TCLClimate::set_target_temperature(float target_temperature) {
  if (std::abs(this->target_temperature - target_temperature) < 0.1f) return;
  // Log temperature change
  ESP_LOGI("TCL", "Target temperature changed to: %.1f°C", target_temperature);

  this->is_changed = true;
  this->target_temperature = target_temperature;
}

void TCLClimate::build_set_cmd(get_cmd_resp_t *get_cmd_resp) {
    memcpy(m_set_cmd.raw, set_cmd_base, sizeof(m_set_cmd.raw));

    // Manual assignment instead of struct initialization
    m_set_cmd.data.power = get_cmd_resp->data.power;
    m_set_cmd.data.off_timer_en = 0;
    m_set_cmd.data.on_timer_en = 0;
    m_set_cmd.data.beep = 1;
    m_set_cmd.data.disp = 1;
    m_set_cmd.data.eco = 0;
    m_set_cmd.data.turbo = get_cmd_resp->data.turbo;
    m_set_cmd.data.mute = get_cmd_resp->data.mute;

    // Mode mapping using lookup table
    static constexpr uint8_t MODE_MAP[] = {
        0x00, // 0x00 - unused
        0x03, // 0x01 -> 0x03
        0x02, // 0x02 -> 0x02 (fan only)
        0x07, // 0x03 -> 0x07 (dry)
        0x01, // 0x04 -> 0x01 (heat)
        0x08  // 0x05 -> 0x08 (auto)
    };

    if (get_cmd_resp->data.mode < sizeof(MODE_MAP)) {
        m_set_cmd.data.mode = MODE_MAP[get_cmd_resp->data.mode];
    }

    // Temperature conversion
    m_set_cmd.data.temp = 15 - get_cmd_resp->data.temp;

    // Fan mode mapping using lookup table
    static constexpr uint8_t FAN_MAP[] = {
        0x00, // 0x00 -> 0x00 (auto)
        0x02, // 0x01 -> 0x02 (speed 1)
        0x03, // 0x02 -> 0x03 (speed 3)
        0x05, // 0x03 -> 0x05 (speed 5)
        0x06, // 0x04 -> 0x06 (speed 2)
        0x07  // 0x05 -> 0x07 (speed 4)
    };

    if (get_cmd_resp->data.fan < sizeof(FAN_MAP)) {
        m_set_cmd.data.fan = FAN_MAP[get_cmd_resp->data.fan];
    }

    // Swing control - extracted from old code
    if (get_cmd_resp->data.vswing_mv) {
      m_set_cmd.data.vswing = 0x07;
      m_set_cmd.data.vswing_fix = 0;
      m_set_cmd.data.vswing_mv = get_cmd_resp->data.vswing_mv;
    } else if (get_cmd_resp->data.vswing_fix) {
      m_set_cmd.data.vswing = 0;
      m_set_cmd.data.vswing_fix = get_cmd_resp->data.vswing_fix;
      m_set_cmd.data.vswing_mv = 0;
    }

    if (get_cmd_resp->data.hswing_mv) {
      m_set_cmd.data.hswing = 0x01;
      m_set_cmd.data.hswing_fix = 0;
      m_set_cmd.data.hswing_mv = get_cmd_resp->data.hswing_mv;
    } else if (get_cmd_resp->data.hswing_fix) {
      m_set_cmd.data.hswing = 0;
      m_set_cmd.data.hswing_fix = get_cmd_resp->data.hswing_fix;
      m_set_cmd.data.hswing_mv = 0;
    }

    m_set_cmd.data.half_degree = 0;

    // Calculate XOR checksum
    uint8_t xor_byte = 0;
    for (size_t i = 0; i < sizeof(m_set_cmd.raw) - 1; i++) {
        xor_byte ^= m_set_cmd.raw[i];
    }
    m_set_cmd.raw[sizeof(m_set_cmd.raw) - 1] = xor_byte;
}

void TCLClimate::setup() {
  set_update_interval(UPDATE_INTERVAL_MS);
  this->set_supported_custom_fan_modes({"Turbo", "Mute", "Automatic", "1", "2", "3", "4", "5"});
}

void TCLClimate::ensure_desired_() {
  // While a command is pending or being verified, keep mutating the same snapshot
  // so temp/mode/fan/swing edits accumulate instead of wiping each other.
  if (!this->desired_valid_) {
    memcpy(this->pending_cmd_resp_.raw, this->m_get_cmd_resp.raw, sizeof(this->pending_cmd_resp_.raw));
    this->desired_valid_ = true;
  }
}

TCLClimate::get_cmd_resp_t *TCLClimate::desired_() {
  this->ensure_desired_();
  return &this->pending_cmd_resp_;
}

void TCLClimate::schedule_set_cmd_() {
  this->last_control_ms_ = millis();
  this->pending_set_cmd_ = true;
  this->awaiting_verify_ = false;
  this->retries_ = 0;
}

// Swing control methods from old code
void TCLClimate::control_vertical_swing(const std::string &swing_mode) {
  ESP_LOGI("TCL", "control_vertical_swing: %s", swing_mode.c_str());

  get_cmd_resp_t *get_cmd_resp = this->desired_();

  get_cmd_resp->data.vswing_mv = 0;
  get_cmd_resp->data.vswing_fix = 0;

  if (swing_mode == "Move full") get_cmd_resp->data.vswing_mv = 0x01;
  else if (swing_mode == "Move upper")  get_cmd_resp->data.vswing_mv = 0x02;
  else if (swing_mode == "Move lower")  get_cmd_resp->data.vswing_mv = 0x03;
  else if (swing_mode == "Fix top") get_cmd_resp->data.vswing_fix = 0x01;
  else if (swing_mode == "Fix upper") get_cmd_resp->data.vswing_fix = 0x02;
  else if (swing_mode == "Fix mid") get_cmd_resp->data.vswing_fix = 0x03;
  else if (swing_mode == "Fix lower") get_cmd_resp->data.vswing_fix = 0x04;
  else if (swing_mode == "Fix bottom") get_cmd_resp->data.vswing_fix = 0x05;

  if (get_cmd_resp->data.vswing_mv) get_cmd_resp->data.vswing = 0x01;
  else get_cmd_resp->data.vswing = 0;

  this->set_vswing_pos(swing_mode);
  schedule_set_cmd_();
}

void TCLClimate::control_horizontal_swing(const std::string &swing_mode) {
  ESP_LOGI("TCL", "control_horizontal_swing: %s", swing_mode.c_str());

  get_cmd_resp_t *get_cmd_resp = this->desired_();

  get_cmd_resp->data.hswing_mv = 0;
  get_cmd_resp->data.hswing_fix = 0;

  if (swing_mode == "Move full") get_cmd_resp->data.hswing_mv = 0x01;
  else if (swing_mode == "Move left") get_cmd_resp->data.hswing_mv = 0x02;
  else if (swing_mode == "Move mid") get_cmd_resp->data.hswing_mv = 0x03;
  else if (swing_mode == "Move right") get_cmd_resp->data.hswing_mv = 0x04;
  else if (swing_mode == "Fix left") get_cmd_resp->data.hswing_fix = 0x01;
  else if (swing_mode == "Fix mid left") get_cmd_resp->data.hswing_fix = 0x02;
  else if (swing_mode == "Fix mid") get_cmd_resp->data.hswing_fix = 0x03;
  else if (swing_mode == "Fix mid right") get_cmd_resp->data.hswing_fix = 0x04;
  else if (swing_mode == "Fix right") get_cmd_resp->data.hswing_fix = 0x05;

  if (get_cmd_resp->data.hswing_mv) get_cmd_resp->data.hswing = 0x01;
  else get_cmd_resp->data.hswing = 0;

  this->set_hswing_pos(swing_mode);
  schedule_set_cmd_();
}

void TCLClimate::control(const climate::ClimateCall &call) {
    get_cmd_resp_t *get_cmd_resp = this->desired_();
    bool should_build_cmd = false;

    if (call.get_mode().has_value()) {
        climate::ClimateMode climate_mode = *call.get_mode();
        ESP_LOGI("TCL", "Received mode control command: %d", static_cast<int>(climate_mode));

        if (climate_mode == climate::CLIMATE_MODE_OFF) {
            get_cmd_resp->data.power = 0x00;
        } else {
            get_cmd_resp->data.power = 0x01;
            switch (climate_mode) {
                case climate::CLIMATE_MODE_COOL:    get_cmd_resp->data.mode = 0x01; break;
                case climate::CLIMATE_MODE_DRY:     get_cmd_resp->data.mode = 0x03; break;
                case climate::CLIMATE_MODE_FAN_ONLY:get_cmd_resp->data.mode = 0x02; break;
                case climate::CLIMATE_MODE_HEAT:
                case climate::CLIMATE_MODE_HEAT_COOL:get_cmd_resp->data.mode = 0x04; break;
                case climate::CLIMATE_MODE_AUTO:    get_cmd_resp->data.mode = 0x05; break;
                default: break;
            }
        }
        should_build_cmd = true;
    }

    if (call.get_target_temperature().has_value()) {
        float temp = *call.get_target_temperature();
        ESP_LOGI("TCL", "Received temperature control command: %.1f°C", temp);

        get_cmd_resp->data.temp = static_cast<uint8_t>(temp + 0.5f) - 16;
        should_build_cmd = true;
    }

    if (call.get_swing_mode().has_value()) {
        climate::ClimateSwingMode swing_mode = *call.get_swing_mode();

        switch(swing_mode) {
            case climate::CLIMATE_SWING_OFF:
                get_cmd_resp->data.hswing = 0;
                get_cmd_resp->data.vswing = 0;
                break;
            case climate::CLIMATE_SWING_BOTH:
                get_cmd_resp->data.hswing = 1;
                get_cmd_resp->data.vswing = 1;
                break;
            case climate::CLIMATE_SWING_VERTICAL:
                get_cmd_resp->data.hswing = 0;
                get_cmd_resp->data.vswing = 1;
                break;
            case climate::CLIMATE_SWING_HORIZONTAL:
                get_cmd_resp->data.hswing = 1;
                get_cmd_resp->data.vswing = 0;
                break;
        }
        should_build_cmd = true;
    }

    // Updated custom fan mode handling
    StringRef custom_fan_mode(call.get_custom_fan_mode());
    if (!custom_fan_mode.empty()) {
        std::string fan_mode(custom_fan_mode.c_str());
        ESP_LOGI("TCL", "Received fan mode control command: %s", fan_mode.c_str());

        get_cmd_resp->data.turbo = 0x00;
        get_cmd_resp->data.mute = 0x00;

        // Use map for fan mode parsing
        static const std::map<std::string, std::pair<uint8_t, uint8_t>> FAN_MODE_MAP = {
            {"Turbo",      {0x03, 0x01}},
            {"Mute",       {0x01, 0x01}},
            {"Automatic",  {0x00, 0x00}},
            {"1",          {0x01, 0x00}},
            {"2",          {0x04, 0x00}},
            {"3",          {0x02, 0x00}},
            {"4",          {0x05, 0x00}},
            {"5",          {0x03, 0x00}}
        };

        auto it = FAN_MODE_MAP.find(fan_mode);
        if (it != FAN_MODE_MAP.end()) {
            get_cmd_resp->data.fan = it->second.first;
            if (fan_mode == "Turbo") get_cmd_resp->data.turbo = 0x01;
            else if (fan_mode == "Mute") get_cmd_resp->data.mute = 0x01;
        }
        should_build_cmd = true;
    }

    if (should_build_cmd) {
        ESP_LOGI("TCL", "Queued command for AC unit (debounced)");

        // Optimistic local state so HA shows the latest user intent immediately.
        this->is_changed = false;
        if (call.get_mode().has_value()) {
          this->set_mode(*call.get_mode());
        }
        if (call.get_target_temperature().has_value()) {
          // Match AC whole-°C storage so UI does not promise fractional setpoints.
          float t = *call.get_target_temperature();
          this->set_target_temperature(static_cast<float>(static_cast<int>(t + 0.5f)));
        }
        if (call.get_swing_mode().has_value()) {
          this->set_swing_mode(*call.get_swing_mode());
        }
        StringRef custom_fan_mode2(call.get_custom_fan_mode());
        if (!custom_fan_mode2.empty()) {
          this->set_custom_fan_mode(custom_fan_mode2);
        }
        if (this->is_changed) {
          this->publish_state();
        }
        schedule_set_cmd_();
    }
}

climate::ClimateTraits TCLClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_COOL,
    climate::CLIMATE_MODE_HEAT,
    climate::CLIMATE_MODE_FAN_ONLY,
    climate::CLIMATE_MODE_DRY,
    climate::CLIMATE_MODE_AUTO
  });
  traits.set_supported_swing_modes({
    climate::CLIMATE_SWING_OFF,
    climate::CLIMATE_SWING_BOTH,
    climate::CLIMATE_SWING_VERTICAL,
    climate::CLIMATE_SWING_HORIZONTAL
  });
  traits.set_visual_min_temperature(16.0);
  traits.set_visual_max_temperature(31.0);
  traits.set_visual_target_temperature_step(1.0);
  return traits;
}

bool TCLClimate::matches_desired_(const get_cmd_resp_t &st) {
    const auto &d = this->pending_cmd_resp_.data;
    const auto &a = st.data;
    if (a.power != d.power) return false;
    if (!d.power) return true;  // off: nothing else matters
    if (a.mode != d.mode) return false;
    if (a.temp != d.temp) return false;
    if (a.fan != d.fan || a.turbo != d.turbo || a.mute != d.mute) return false;
    if ((d.vswing_mv || d.vswing_fix) && (a.vswing_mv != d.vswing_mv || a.vswing_fix != d.vswing_fix)) return false;
    if ((d.hswing_mv || d.hswing_fix) && (a.hswing_mv != d.hswing_mv || a.hswing_fix != d.hswing_fix)) return false;
    return true;
}

void TCLClimate::send_desired_(uint32_t now) {
    build_set_cmd(&this->pending_cmd_resp_);
    write_array(m_set_cmd.raw, sizeof(m_set_cmd.raw));
    this->pending_set_cmd_ = false;
    this->awaiting_verify_ = true;
    this->last_send_ms_ = now;
    this->skip_polls_ = 1;  // give the AC a quiet gap to process the command
    ESP_LOGI("TCL", "Sent set command (attempt %u)", static_cast<unsigned>(this->retries_ + 1));
}

void TCLClimate::update() {
    const uint32_t now = millis();
    if (this->skip_polls_ > 0) {
        this->skip_polls_--;
        return;
    }
    if (this->pending_set_cmd_ && (now - this->last_control_ms_ >= CONTROL_DEBOUNCE_MS)) {
        if (!this->awaiting_verify_) this->retries_ = 0;
        this->send_desired_(now);
        return;
    }
    write_array(REQ_CMD, sizeof(REQ_CMD));
}

int TCLClimate::read_data_line(int readch, uint8_t *buffer, int len) {
    static int pos = 0;
    static bool wait_len = false;
    static int skipch = 0;

    if (readch < 0) return -1;

    if (readch == 0xBB && skipch == 0 && !wait_len) {
        pos = 0;
        skipch = 3; // wait for length byte
        wait_len = true;
        if (pos < len) buffer[pos++] = static_cast<uint8_t>(readch);
    } else if (skipch == 0 && wait_len) {
        if (pos < len) buffer[pos++] = static_cast<uint8_t>(readch);
        skipch = readch + 1; // +1 for checksum
        wait_len = false;
    } else if (skipch > 0) {
        if (pos < len) buffer[pos++] = static_cast<uint8_t>(readch);
        if (--skipch == 0 && !wait_len) return pos;
    }

    return -1;
}

bool TCLClimate::is_valid_xor(uint8_t *buffer, int len) {
    if (len < 1) return false;

    uint8_t xor_byte = 0;
    for (int i = 0; i < len - 1; i++) {
        xor_byte ^= buffer[i];
    }
    return xor_byte == buffer[len - 1];
}

void TCLClimate::print_hex_str(uint8_t *buffer, int len) {
    if (len <= 0) return;

    char str[MAX_LINE_LENGTH * 3] = {0};
    char *pstr = str;

    for (int i = 0; i < len && (pstr - str) < sizeof(str) - 3; i++) {
        pstr += sprintf(pstr, "%02X ", buffer[i]);
    }

    ESP_LOGD("TCL", "Received: %s", str);
}

void TCLClimate::loop() {
    static uint8_t buffer[MAX_LINE_LENGTH];

    while (available()) {
        int len = read_data_line(read(), buffer, MAX_LINE_LENGTH);
        if (len == sizeof(m_get_cmd_resp) && buffer[3] == 0x04) {
            if (is_valid_xor(buffer, len)) {
                memcpy(m_get_cmd_resp.raw, buffer, len);
                print_hex_str(buffer, len);

                // Calculate current temperature
                float curr_temp = ((static_cast<uint16_t>(buffer[17] << 8) | buffer[18]) / 374.0f - 32.0f) / 1.8f;
                this->is_changed = false;

                bool apply_status = true;
                if (this->desired_valid_) {
                    const uint32_t now = millis();
                    if (this->pending_set_cmd_) {
                        apply_status = false;  // still collecting a burst of edits
                    } else if (this->awaiting_verify_) {
                        if (now - this->last_send_ms_ < VERIFY_DELAY_MS) {
                            apply_status = false;
                        } else if (this->matches_desired_(m_get_cmd_resp)) {
                            ESP_LOGI("TCL", "AC confirmed requested state");
                            this->awaiting_verify_ = false;
                        } else if (this->retries_ < MAX_RETRIES) {
                            this->retries_++;
                            ESP_LOGW("TCL", "AC state differs from request, resending (%u/%u)", static_cast<unsigned>(this->retries_), static_cast<unsigned>(MAX_RETRIES));
                            this->pending_set_cmd_ = true;
                            this->last_control_ms_ = now - CONTROL_DEBOUNCE_MS;
                            apply_status = false;
                        } else {
                            ESP_LOGW("TCL", "AC did not accept command after %u retries; showing real AC state", static_cast<unsigned>(MAX_RETRIES));
                            this->awaiting_verify_ = false;
                        }
                    }
                }

                if (apply_status) {
                    this->desired_valid_ = false;

                    // Set mode
                    if (m_get_cmd_resp.data.power == 0x00) {
                        this->set_mode(climate::CLIMATE_MODE_OFF);
                    } else {
                        static const std::map<uint8_t, climate::ClimateMode> MODE_MAP = {
                            {0x01, climate::CLIMATE_MODE_COOL},
                            {0x03, climate::CLIMATE_MODE_DRY},
                            {0x02, climate::CLIMATE_MODE_FAN_ONLY},
                            {0x04, climate::CLIMATE_MODE_HEAT},
                            {0x05, climate::CLIMATE_MODE_AUTO}
                        };
                        auto it = MODE_MAP.find(m_get_cmd_resp.data.mode);
                        if (it != MODE_MAP.end()) {
                            this->set_mode(it->second);
                        }
                    }

                    // Set fan mode
                    static const std::map<uint8_t, std::string> FAN_MODE_MAP = {
                        {0x00, "Automatic"},
                        {0x01, "1"},
                        {0x04, "2"},
                        {0x02, "3"},
                        {0x05, "4"},
                        {0x03, "5"}
                    };

                    if (m_get_cmd_resp.data.turbo) {
                      // String literal to StringRef - use explicit construction
                      this->set_custom_fan_mode(StringRef("Turbo"));
                    } else if (m_get_cmd_resp.data.mute) {
                      this->set_custom_fan_mode(StringRef("Mute"));
                    } else {
                      auto it = FAN_MODE_MAP.find(m_get_cmd_resp.data.fan);
                      if (it != FAN_MODE_MAP.end()) {
                        StringRef current_fan(StringRef(this->get_custom_fan_mode()));
                        if (current_fan.empty() || current_fan != it->second) {
                          // Convert std::string to StringRef
                          this->set_custom_fan_mode(StringRef(it->second.c_str(), it->second.size()));
                        }
                      }
                    }

                    // Set swing mode - extracted from old code
                    if (m_get_cmd_resp.data.hswing && m_get_cmd_resp.data.vswing) {
                        this->set_swing_mode(climate::CLIMATE_SWING_BOTH);
                    } else if (!m_get_cmd_resp.data.hswing && !m_get_cmd_resp.data.vswing) {
                        this->set_swing_mode(climate::CLIMATE_SWING_OFF);
                    } else if (m_get_cmd_resp.data.vswing) {
                        this->set_swing_mode(climate::CLIMATE_SWING_VERTICAL);
                    } else if (m_get_cmd_resp.data.hswing) {
                        this->set_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
                    }

                    // Set swing positions - extracted from old code
                    if (m_get_cmd_resp.data.vswing_mv == 0x01) set_vswing_pos("Move full");
                    else if (m_get_cmd_resp.data.vswing_mv == 0x02) set_vswing_pos("Move upper");
                    else if (m_get_cmd_resp.data.vswing_mv == 0x03) set_vswing_pos("Move lower");
                    else if (m_get_cmd_resp.data.vswing_fix == 0x01) set_vswing_pos("Fix top");
                    else if (m_get_cmd_resp.data.vswing_fix == 0x02) set_vswing_pos("Fix upper");
                    else if (m_get_cmd_resp.data.vswing_fix == 0x03) set_vswing_pos("Fix mid");
                    else if (m_get_cmd_resp.data.vswing_fix == 0x04) set_vswing_pos("Fix lower");
                    else if (m_get_cmd_resp.data.vswing_fix == 0x05) set_vswing_pos("Fix bottom");
                    else set_vswing_pos("Last position");

                    if (m_get_cmd_resp.data.hswing_mv == 0x01) set_hswing_pos("Move full");
                    else if (m_get_cmd_resp.data.hswing_mv == 0x02) set_hswing_pos("Move left");
                    else if (m_get_cmd_resp.data.hswing_mv == 0x03) set_hswing_pos("Move mid");
                    else if (m_get_cmd_resp.data.hswing_mv == 0x04) set_hswing_pos("Move right");
                    else if (m_get_cmd_resp.data.hswing_fix == 0x01) set_hswing_pos("Fix left");
                    else if (m_get_cmd_resp.data.hswing_fix == 0x02) set_hswing_pos("Fix mid left");
                    else if (m_get_cmd_resp.data.hswing_fix == 0x03) set_hswing_pos("Fix mid");
                    else if (m_get_cmd_resp.data.hswing_fix == 0x04) set_hswing_pos("Fix mid right");
                    else if (m_get_cmd_resp.data.hswing_fix == 0x05) set_hswing_pos("Fix right");
                    else set_hswing_pos("Last position");

                    this->set_target_temperature(static_cast<float>(m_get_cmd_resp.data.temp + 16));
                }

                this->set_current_temperature(curr_temp);

                if (this->is_changed) {
                    this->publish_state();
                }
            }
        }
    }
}

}  // namespace tcl_climate
}  // namespace esphome
//#endif  // USE_ARDUINO
