#include "huawei_r4875g1.h"
#include "esphome/core/log.h"

namespace esphome {
namespace huawei_r4875g1 {

static const char *const TAG = "huawei_r4875g1";

// 状态请求帧
static const uint32_t STATUS_REQUEST_ID = 0x108040FE;
// 状态响应帧
static const uint32_t STATUS_RESPONSE_ID = 0x1081407F;
// 设置命令帧
static const uint32_t SET_COMMAND_ID = 0x108180FE;

void HuaweiR4875G1::setup() {
  // 注册CAN监听
  if (canbus_ == nullptr) {
    ESP_LOGE(TAG, "CAN bus pointer is null!");
    return;
  }
  canbus_->add_listener(this);

  // 设置CAN过滤器（只接收状态响应帧）
  canbus::CanFilter filter = {
    .can_id = STATUS_RESPONSE_ID,
    .can_mask = 0x1FFFFFFF,  // 标准帧和扩展帧
    .extended = true
  };
  canbus_->add_filter(filter);

  // 初始请求状态
  request_status_();
}

void HuaweiR4875G1::loop() {
  uint32_t now = millis();
  if (now - last_update_ >= update_interval_) {
    last_update_ = now;
    request_status_();
  }
  // 可以在此处增加掉线报警处理
}

void HuaweiR4875G1::on_can_message(const canbus::CanFrame &frame) {
  if (frame.can_id == STATUS_RESPONSE_ID && frame.data.size() >= 8) {
    last_frame_time_ = millis();
    process_status_frame_(frame);
  }
}

void HuaweiR4875G1::request_status_() {
  // 发送状态请求帧：8字节全0
  std::vector<uint8_t> data(8, 0x00);
  send_command_(STATUS_REQUEST_ID, data);
}

void HuaweiR4875G1::process_status_frame_(const canbus::CanFrame &frame) {
  const uint8_t *data = frame.data.data();
  uint8_t param_id = data[1];
  float value = 0.0f;

  switch (param_id) {
    case 0x70:  // Input Power
      value = parse_value_(data, 1024.0f, OUTPUT_POWER);
      sensor_values_[OUTPUT_POWER] = value;
      break;
    case 0x71:  // Input Frequency
      value = parse_value_(data, 1024.0f, INPUT_FREQUENCY);
      sensor_values_[INPUT_FREQUENCY] = value;
      break;
    case 0x72:  // Input Current
      value = parse_value_(data, 1024.0f, INPUT_CURRENT);
      sensor_values_[INPUT_CURRENT] = value;
      break;
    case 0x73:  // Output Power (DC)
      value = parse_value_(data, 1024.0f, OUTPUT_POWER);
      sensor_values_[OUTPUT_POWER] = value;
      break;
    case 0x75:  // Output Voltage
      value = parse_value_(data, 1024.0f, OUTPUT_VOLTAGE);
      sensor_values_[OUTPUT_VOLTAGE] = value;
      break;
    case 0x78:  // Input Voltage
      value = parse_value_(data, 1024.0f, INPUT_VOLTAGE);
      sensor_values_[INPUT_VOLTAGE] = value;
      break;
    case 0x7F:  // Output Temperature
      value = parse_value_(data, 1024.0f, TEMPERATURE);
      sensor_values_[TEMPERATURE] = value;
      break;
    case 0x81:  // Output Current
      value = parse_value_(data, 1024.0f, OUTPUT_CURRENT);
      sensor_values_[OUTPUT_CURRENT] = value;
      break;
    default:
      ESP_LOGV(TAG, "Unhandled parameter ID: 0x%02X", param_id);
      break;
  }
}

float HuaweiR4875G1::parse_value_(const uint8_t *data, float divisor, SensorType sensor_type) {
  if (divisor == 0.0f) {
    ESP_LOGE(TAG, "Divisor is zero for sensor_type %d", int(sensor_type));
    return 0.0f;
  }
  uint32_t raw = (uint32_t(data[4]) << 24) | 
                (uint32_t(data[5]) << 16) | 
                (uint32_t(data[6]) << 8) | 
                data[7];
  float value = raw / divisor;
  if (sensor_type < 0 || sensor_type >= SENSOR_TYPE_COUNT) {
    ESP_LOGW(TAG, "Calibration sensor_type %d out of range!", int(sensor_type));
    return value;
  }
  Calibration calib = calibrations_[sensor_type];
  return (value * calib.multiplier) + calib.offset;
}

void HuaweiR4875G1::send_command_(uint32_t can_id, const std::vector<uint8_t> &data) {
  if (canbus_ == nullptr) {
    ESP_LOGE(TAG, "CAN bus pointer is null!");
    return;
  }
  canbus::CanFrame frame;
  frame.can_id = can_id;
  frame.extended = true;
  frame.data = data;
  canbus_->send_data(frame);
}

void HuaweiR4875G1::set_input_voltage_calibration(float mult, float offset) {
  calibrations_[INPUT_VOLTAGE] = {mult, offset};
}

void HuaweiR4875G1::set_output_voltage_calibration(float mult, float offset) {
  calibrations_[OUTPUT_VOLTAGE] = {mult, offset};
}

void HuaweiR4875G1::set_input_current_calibration(float mult, float offset) {
  calibrations_[INPUT_CURRENT] = {mult, offset};
}

void HuaweiR4875G1::set_output_current_calibration(float mult, float offset) {
  calibrations_[OUTPUT_CURRENT] = {mult, offset};
}

void HuaweiR4875G1::set_voltage_limits(float min, float max) {
  min_voltage_ = min;
  max_voltage_ = max;
}

void HuaweiR4875G1::set_current_limits(float min, float max) {
  min_current_ = min;
  max_current_ = max;
}

void HuaweiR4875G1::set_voltage(float voltage) {
  if (voltage < min_voltage_ || voltage > max_voltage_) {
    ESP_LOGE(TAG, "Voltage %.1fV is out of range [%.1f, %.1f]", voltage, min_voltage_, max_voltage_);
    return;
  }
  send_voltage_setting_(voltage);
  current_voltage_ = voltage;
}

void HuaweiR4875G1::set_current(float current) {
  if (current < min_current_ || current > max_current_) {
    ESP_LOGE(TAG, "Current %.1fA is out of range [%.1f, %.1f]", current, min_current_, max_current_);
    return;
  }
  send_current_setting_(current);
  current_current_ = current;
}

void HuaweiR4875G1::enable() {
  send_control_command_(0x00); // 启用命令
  enabled_ = true;
}

void HuaweiR4875G1::disable() {
  send_control_command_(0x01); // 禁用命令
  enabled_ = false;
}

void HuaweiR4875G1::send_voltage_setting_(float voltage) {
  uint16_t deci_volts = static_cast<uint16_t>(voltage * 10);
  uint32_t value = deci_volts * 102;
  
  std::vector<uint8_t> data = {
    0x01, 0x00, 0x00, 0x00,
    static_cast<uint8_t>((value >> 24) & 0xFF),
    static_cast<uint8_t>((value >> 16) & 0xFF),
    static_cast<uint8_t>((value >> 8) & 0xFF),
    static_cast<uint8_t>(value & 0xFF)
  };
  
  send_command_(SET_COMMAND_ID, data);
}

void HuaweiR4875G1::send_current_setting_(float current) {
  uint16_t deci_amps = static_cast<uint16_t>(current * 10);
  uint32_t value = static_cast<uint32_t>(deci_amps * 1.5f);
  
  std::vector<uint8_t> data = {
    0x01, 0x03, 0x00, 0x00,
    static_cast<uint8_t>((value >> 24) & 0xFF),
    static_cast<uint8_t>((value >> 16) & 0xFF),
    static_cast<uint8_t>((value >> 8) & 0xFF),
    static_cast<uint8_t>(value & 0xFF)
  };
  
  send_command_(SET_COMMAND_ID, data);
}

void HuaweiR4875G1::send_control_command_(uint8_t command) {
  std::vector<uint8_t> data = {
    0x01, 0x32, 0x00, command, 
    0x00, 0x00, 0x00, 0x00
  };
  send_command_(SET_COMMAND_ID, data);
}

float HuaweiR4875G1::get_sensor_value(SensorType sensor_type) {
  if (sensor_type >= 0 && sensor_type < SENSOR_TYPE_COUNT) {
    return sensor_values_[sensor_type];
  }
  ESP_LOGW(TAG, "Sensor type %d out of range!", int(sensor_type));
  return 0.0f;
}

}  // namespace huawei_r4875g1
}  // namespace esphome