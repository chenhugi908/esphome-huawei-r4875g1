#pragma once

namespace esphome {
namespace huawei_r4875g1 {

// 支持的传感器类型。必须和Python侧保持一致。
enum SensorType {
  INPUT_VOLTAGE = 0,
  INPUT_CURRENT,
  INPUT_FREQUENCY,
  OUTPUT_VOLTAGE,
  OUTPUT_CURRENT,
  OUTPUT_POWER,
  TEMPERATURE,
  SENSOR_TYPE_COUNT  // 必须放在最后
};

}  // namespace huawei_r4875g1
}  // namespace esphome