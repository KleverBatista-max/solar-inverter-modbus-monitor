#include "modbus_sniffer.h"
#include "esphome/core/log.h"

namespace esphome {
namespace modbus_sniffer {

static const char *const TAG = "modbus_sniffer";

uint16_t ModbusSniffer::crc16_modbus_(const uint8_t *buf, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t pos = 0; pos < len; pos++) {
    crc ^= (uint16_t) buf[pos];
    for (int i = 0; i < 8; i++) {
      if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
      else crc >>= 1;
    }
  }
  return crc;
}

void ModbusSniffer::publish_reg_(uint16_t addr, uint16_t val) {

  ESP_LOGW(TAG, "REG 0x%04X = %u", addr, val);

  if (addr == 0x0100) {
    float v = (float) val;
    ESP_LOGI(TAG, "SOC = %.0f %%", v);
    if (soc_) soc_->publish_state(v);

  } else if (addr == 0x0101) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "BAT_V = %.1f V", v);
    if (bat_v_) bat_v_->publish_state(v);

  } else if (addr == 0x0102) {
    int16_t battI = (int16_t) val;
    float v = battI * 0.1f;
    ESP_LOGI(TAG, "BAT_I = %.1f A", v);
    if (bat_i_) bat_i_->publish_state(v);

  } else if (addr == 0x0107) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "PV1_V = %.1f V", v);
    if (pv1_v_) pv1_v_->publish_state(v);

  } else if (addr == 0x0108) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "PV1_I = %.1f A", v);
    if (pv1_i_) pv1_i_->publish_state(v);

  } else if (addr == 0x0109) {
    float v = (float) val;
    ESP_LOGI(TAG, "PV1_P = %.0f W", v);
    if (mppt1_) mppt1_->publish_state(v);

  } else if (addr == 0x010F) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "PV2_V = %.1f V", v);
    if (pv2_v_) pv2_v_->publish_state(v);

  } else if (addr == 0x0110) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "PV2_I = %.1f A", v);
    if (pv2_i_) pv2_i_->publish_state(v);

  } else if (addr == 0x0111) {
    float v = (float) val;
    ESP_LOGI(TAG, "PV2_P = %.0f W", v);
    if (mppt2_) mppt2_->publish_state(v);

  } else if (addr == 0x0213) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "GRID_A_V = %.1f V", v);
    if (grid_a_v_) grid_a_v_->publish_state(v);

  } else if (addr == 0x0214) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "GRID_A_I = %.1f A", v);
    if (grid_a_i_) grid_a_i_->publish_state(v);

  } else if (addr == 0x0215) {
    float v = val * 0.01f;
    ESP_LOGI(TAG, "GRID_FREQ = %.2f Hz", v);
    if (grid_freq_) grid_freq_->publish_state(v);

  } else if (addr == 0x022A) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "GRID_B_V = %.1f V", v);
    if (grid_b_v_) grid_b_v_->publish_state(v);

  } else if (addr == 0x0238) {
    float v = val * 0.1f;
    ESP_LOGI(TAG, "GRID_B_I = %.1f A", v);
    if (grid_b_i_) grid_b_i_->publish_state(v);

  } else if (addr == 0x021B) {
    float v = (float) val;
    ESP_LOGI(TAG, "LOAD_21B = %.0f W", v);
    if (load21b_) load21b_->publish_state(v);

  } else if (addr == 0x0232) {
    float v = (float) val;
    ESP_LOGI(TAG, "LOAD_232 = %.0f W", v);
    if (load232_) load232_->publish_state(v);

  } else if (addr == 0x0037) {
    float v = (float) val;
    ESP_LOGI(TAG, "INV_TEMP = %.0f C", v);
    if (inv_temp_) inv_temp_->publish_state(v);

  } else if (addr == 0x0038) {
    float v = (float) val;
    ESP_LOGI(TAG, "RAD_TEMP = %.0f C", v);
    if (rad_temp_) rad_temp_->publish_state(v);
  }
}

void ModbusSniffer::handle_frame_(uint8_t *f, size_t n) {
  if (n < 5) return;

  // CRC check
  uint16_t crc_rx = (uint16_t) f[n - 2] | ((uint16_t) f[n - 1] << 8);
  uint16_t crc_ok = crc16_modbus_(f, n - 2);
  if (crc_rx != crc_ok) return;

  uint8_t id = f[0];
  uint8_t fn = f[1];

  if (id != slave_id_) return;

  // =========================
  // Request (Master -> Slave)
  // 0x03 Holding / 0x04 Input
  // =========================
  if ((fn == 0x03 || fn == 0x04) && n == 8) {
    last_fn_   = fn;
    last_addr_ = ((uint16_t) f[2] << 8) | f[3];
    last_qty_  = ((uint16_t) f[4] << 8) | f[5];
    have_last_req_ = true;
    return;
  }

  // =========================
  // Response (Slave -> Master)
  // 0x03 / 0x04
  // =========================
  if ((fn == 0x03 || fn == 0x04) && n >= 5 && have_last_req_) {
    // Ensure response matches last request type
    if (fn != last_fn_) return;

    uint8_t bc = f[2];
    if ((size_t) bc + 5 != n) return;

    uint16_t addr = last_addr_;
    uint16_t regs = bc / 2;

    // Optional safety: don't decode more than requested
    if (last_qty_ > 0 && regs > last_qty_) regs = last_qty_;

    for (uint16_t i = 0; i < regs; i++) {
      size_t idx = 3 + (i * 2);
      if (idx + 1 >= n - 2) break;

      uint16_t val = ((uint16_t) f[idx] << 8) | f[idx + 1];
      publish_reg_(addr + i, val);
    }

    // IMPORTANT: clear the last request so we don't mis-attribute later frames
    have_last_req_ = false;
  }
}

void ModbusSniffer::setup() {
  ESP_LOGI(TAG, "🔥🔥🔥 BUILD_TEST_MPPT_V9999 🔥🔥🔥");
  ESP_LOGI(TAG, "Sniffer Modbus RS485 (RX only) iniciado");
  ESP_LOGI(TAG, "slave_id=%u frame_gap_ms=%u", slave_id_, (unsigned) frame_gap_ms_);
}

void ModbusSniffer::loop() {
  if (uart_ == nullptr) return;

  while (uart_->available()) {
    uint8_t b;
    uart_->read_byte(&b);
    bytes_in_window_++;

    if (len_ < sizeof(buf_)) buf_[len_++] = b;
    last_byte_ms_ = millis();
  }

  uint32_t now = millis();

  if (now - last_report_ms_ >= 1000) {
    ESP_LOGD(TAG, "bytes/sec = %u", (unsigned) bytes_in_window_);
    bytes_in_window_ = 0;
    last_report_ms_ = now;
  }

  if (len_ > 0 && (now - last_byte_ms_) > frame_gap_ms_) {
    handle_frame_(buf_, len_);
    len_ = 0;
  }
}

}  // namespace modbus_sniffer
}  // namespace esphome