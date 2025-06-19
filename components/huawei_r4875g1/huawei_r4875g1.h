#pragma once

#include "esphome.h"
#include "esphome/components/canbus/canbus.h"
#include "sensor_types.h"

namespace esphome {
namespace huawei_r4875g1 {

class HuaweiR4875G1 : public Component, public canbus::CanbusListener {
 public:
  void set_canbus(canbus::Canbus *canbus) { canbus_ = canbus; }
  void set_update_interval(uint32_t interval) { update_interval_ = interval; }
  
  // 传感器校准
  void set_input_voltage_calibration(float mult, float offset);
  void set_output_voltage_calibration(float mult, float offset);
  void set_input_current_calibration(float mult, float offset);
  void set_output_current_calibration(float mult, float offset);
  
  // 设置范围
  void set_voltage_limits(float min, float max);
  void set_current_limits(float min, float max);

  // 控制方法
  void set_voltage(float voltage);
  void set_current(float current);
  void enable();
  void disable();

  // 传感器接口
  float get_sensor_value(SensorType sensor_type);

  // 状态获取
  bool is_enabled() const { return enabled_; }
  // 是否掉线
  bool is_online() const { return (millis() - last_frame_time_) < offline_timeout_; }

  void setup() override;
  void loop() override;

 protected:
  void on_can_message(const canbus::CanFrame &frame) override;
  void send_command_(uint32_t can_id, const std::vector<uint8_t> &data);
  void request_status_();
  void process_status_frame_(const canbus::CanFrame &frame);
  void send_voltage_setting_(float voltage);
  void send_current_setting_(float current);
  void send_control_command_(uint8_t command);

  // 参数解析
  float parse_value_(const uint8_t *data, float divisor, SensorType sensor_type);

  canbus::Canbus *canbus_{nullptr};
  uint32_t update_interval_{1000};
  uint32_t last_update_{0};
  bool enabled_{false};
  
  // 传感器值存储
  float sensor_values_[SENSOR_TYPE_COUNT] = {0};
  
  // 校准参数
  struct Calibration {
    float multiplier{1.0f};
    float offset{0.0f};
  } calibrations_[SENSOR_TYPE_COUNT];
  
  // 设置范围
  float min_voltage_{41.5f};
  float max_voltage_{58.5f};
  float min_current_{0.0f};
  float max_current_{90.5f};
  
  // 当前设置值
  float current_voltage_{53.5f};
  float current_current_{50.0f};

  // 掉线检测
  uint32_t last_frame_time_{0};
  uint32_t offline_timeout_{5000}; // 超过5s未收到CAN帧判定为掉线
};

// 传感器组件
class HuaweiSensor : public sensor::Sensor, public PollingComponent {
 public:
  HuaweiSensor(HuaweiR4875G1 *parent, SensorType type) 
    : PollingComponent(1000), parent_(parent), type_(type) {}
  
  void update() override {
    float value = parent_->get_sensor_value(type_);
    publish_state(value);
  }
  
 protected:
  HuaweiR4875G1 *parent_;
  SensorType type_;
};

// 状态传感器
class HuaweiBinarySensor : public binary_sensor::BinarySensor, public PollingComponent {
 public:
  HuaweiBinarySensor(HuaweiR4875G1 *parent) : PollingComponent(1000), parent_(parent) {}
  
  void update() override {
    // 如果设备掉线，报告为不可用
    publish_state(parent_->is_enabled() && parent_->is_online());
  }
  
 protected:
  HuaweiR4875G1 *parent_;
};

}  // namespace huawei_r4875g1
}  // namespace esphome