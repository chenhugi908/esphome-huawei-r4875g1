import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import canbus, sensor, binary_sensor
from esphome import automation
from esphome.automation import maybe_simple_id

# 下面这些常量必须手动定义，ESPHome 2025.4.2 以后已不再提供
CONF_INPUT_VOLTAGE = "input_voltage"
CONF_OUTPUT_VOLTAGE = "output_voltage"
CONF_INPUT_CURRENT = "input_current"
CONF_OUTPUT_CURRENT = "output_current"
CONF_MIN_VOLTAGE = "min"
CONF_MAX_VOLTAGE = "max"
CONF_MIN_CURRENT = "min"
CONF_MAX_CURRENT = "max"
CONF_ID = "id"
CONF_UPDATE_INTERVAL = "update_interval"

# ESPHome 2025.4.2 后已移除的常量
CONF_CANBUS_ID = "canbus_id"

CODEOWNERS = ["@chenhugi908"]
DEPENDENCIES = ["canbus"]
AUTO_LOAD = ["sensor", "binary_sensor"]

# ...（后续代码保持不变）