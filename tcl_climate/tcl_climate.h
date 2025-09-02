#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace tcl_climate {

class TCLClimate : public climate::Climate, public uart::UARTDevice, public PollingComponent {
 public:
  TCLClimate(uart::UARTComponent *parent) : UARTDevice(parent) {}
  TCLClimate() : PollingComponent() {}  // Default constructor for PollingComponent

  bool is_changed : 1;
  // Swing position strings
  std::string hswing_pos = "";
  std::string vswing_pos = "";

  union get_cmd_resp_t {
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

      uint8_t vswing_fix : 3;
      uint8_t vswing_mv : 2;
      uint8_t byte_51_bit_5_7 : 3;

      uint8_t hswing_fix : 3;
      uint8_t hswing_mv : 3;
      uint8_t byte_52_bit_6_7 : 2;

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
      uint8_t vswing_fix : 3;
      uint8_t vswing_mv : 2;
      uint8_t byte_32_bit_5_7 : 3;

      uint8_t hswing_fix : 3;
      uint8_t hswing_mv : 3;
      uint8_t byte_33_bit_6_7 : 2;

      uint8_t xor_sum;
    } data;
    uint8_t raw[35];
  };

  bool ready_to_send_set_cmd_flag = false;

  uint8_t set_cmd_base[35] = {0xBB, 0x00, 0x01, 0x03, 0x1D, 0x00, 0x00, 0x64, 0x03, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  get_cmd_resp_t m_get_cmd_resp = {0};
  set_cmd_t m_set_cmd = {0};

  void set_current_temperature(float current_temperature);
  void set_custom_fan_mode(const std::string &fan_mode);
  void set_mode(climate::ClimateMode mode);
  void set_swing_mode(climate::ClimateSwingMode swing_mode);
  void set_target_temperature(float target_temperature);

  void set_hswing_pos(const std::string &hswing_pos);
  void set_vswing_pos(const std::string &vswing_pos);
  // Swing control methods
  void control_vertical_swing(const std::string &swing_mode);
  void control_horizontal_swing(const std::string &swing_mode);

  void build_set_cmd(get_cmd_resp_t *get_cmd_resp);

  void setup() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void update() override;
  void loop() override;

 private:
  int read_data_line(int readch, uint8_t *buffer, int len);
  bool is_valid_xor(uint8_t *buffer, int len);
  void print_hex_str(uint8_t *buffer, int len);
};

}  // namespace tcl_climate
}  // namespace esphome
