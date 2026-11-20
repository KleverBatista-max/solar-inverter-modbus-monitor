#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace modbus_sniffer {

class ModbusSniffer : public Component {
 public:
  void set_uart(uart::UARTComponent *u) { uart_ = u; }
  void set_slave_id(uint8_t id) { slave_id_ = id; }
  void set_frame_gap_ms(uint32_t ms) { frame_gap_ms_ = ms; }

  // Sensor setters
  void set_soc_sensor(sensor::Sensor *s) { soc_ = s; }
  void set_bat_v_sensor(sensor::Sensor *s) { bat_v_ = s; }
  void set_bat_i_sensor(sensor::Sensor *s) { bat_i_ = s; }
  void set_mppt1_sensor(sensor::Sensor *s) { mppt1_ = s; }
  void set_mppt2_sensor(sensor::Sensor *s) { mppt2_ = s; }
  void set_load21b_sensor(sensor::Sensor *s) { load21b_ = s; }
  void set_load232_sensor(sensor::Sensor *s) { load232_ = s; }
  void set_pv1_v_sensor(sensor::Sensor *s) { pv1_v_ = s; }
  void set_pv1_i_sensor(sensor::Sensor *s) { pv1_i_ = s; }
  void set_pv2_v_sensor(sensor::Sensor *s) { pv2_v_ = s; }
  void set_pv2_i_sensor(sensor::Sensor *s) { pv2_i_ = s; }
  void set_grid_v_sensor(sensor::Sensor *s) { grid_v_ = s; }
  void set_grid_p_sensor(sensor::Sensor *s) { grid_p_ = s; }
  void set_inv_temp_sensor(sensor::Sensor *s) { inv_temp_ = s; }
  void set_rad_temp_sensor(sensor::Sensor *s) { rad_temp_ = s; }
  void set_grid_a_v_sensor(sensor::Sensor *s) { grid_a_v_ = s; }
  void set_grid_a_i_sensor(sensor::Sensor *s) { grid_a_i_ = s; }
  void set_grid_freq_sensor(sensor::Sensor *s) { grid_freq_ = s; }
  void set_grid_b_v_sensor(sensor::Sensor *s) { grid_b_v_ = s; }
  void set_grid_b_i_sensor(sensor::Sensor *s) { grid_b_i_ = s; }
  
  void setup() override;
  void loop() override;

 protected:
  uart::UARTComponent *uart_{nullptr};

  // sensors
  sensor::Sensor *soc_{nullptr};
  sensor::Sensor *bat_v_{nullptr};
  sensor::Sensor *bat_i_{nullptr};
  sensor::Sensor *mppt1_{nullptr};
  sensor::Sensor *mppt2_{nullptr};
  sensor::Sensor *load21b_{nullptr};
  sensor::Sensor *load232_{nullptr};
  sensor::Sensor *pv1_v_{nullptr};
  sensor::Sensor *pv1_i_{nullptr};
  sensor::Sensor *pv2_v_{nullptr};
  sensor::Sensor *pv2_i_{nullptr};
  sensor::Sensor *grid_v_{nullptr};
  sensor::Sensor *grid_p_{nullptr};
  sensor::Sensor *inv_temp_{nullptr};
  sensor::Sensor *rad_temp_{nullptr};
  sensor::Sensor *grid_a_v_{nullptr};
  sensor::Sensor *grid_a_i_{nullptr};
  sensor::Sensor *grid_freq_{nullptr};
  sensor::Sensor *grid_b_v_{nullptr};
  sensor::Sensor *grid_b_i_{nullptr};

  uint16_t last_addr_{0};
  uint16_t last_qty_{0};
  uint8_t  last_fn_{0};
  bool have_last_req_{false};

  uint8_t buf_[256];
  size_t len_{0};
  uint32_t last_byte_ms_{0};

  uint32_t last_report_ms_{0};
  uint32_t bytes_in_window_{0};

  uint8_t slave_id_{1};
  uint32_t frame_gap_ms_{8};

  uint16_t crc16_modbus_(const uint8_t *buf, size_t len);
  void handle_frame_(uint8_t *f, size_t n);
  void publish_reg_(uint16_t addr, uint16_t val);
};

}  // namespace modbus_sniffer
}  // namespace esphome