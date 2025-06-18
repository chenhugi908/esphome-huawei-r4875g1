#pragma once

namespace esphome {
namespace huawei_r4875g1 {

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