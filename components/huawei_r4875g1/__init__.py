import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import canbus, sensor, binary_sensor
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.const import (
    CONF_ID,
    CONF_UPDATE_INTERVAL,
    CONF_INPUT_VOLTAGE,
    CONF_OUTPUT_VOLTAGE,
    CONF_INPUT_CURRENT,
    CONF_OUTPUT_CURRENT,
    CONF_MIN_VOLTAGE,
    CONF_MAX_VOLTAGE,
    CONF_MIN_CURRENT,
    CONF_MAX_CURRENT,
)

# ESPHome 2025.4.2 后已移除的常量
CONF_CANBUS_ID = "canbus_id"

CODEOWNERS = ["@chenhugi908"]
DEPENDENCIES = ["canbus"]
AUTO_LOAD = ["sensor", "binary_sensor"]

huawei_r4875g1_ns = cg.esphome_ns.namespace("huawei_r4875g1")
HuaweiR4875G1 = huawei_r4875g1_ns.class_(
    "HuaweiR4875G1", cg.Component, canbus.CanbusListener
)

SensorType = huawei_r4875g1_ns.enum("SensorType")
SENSOR_TYPES = {
    "input_voltage": SensorType.INPUT_VOLTAGE,
    "input_current": SensorType.INPUT_CURRENT,
    "input_frequency": SensorType.INPUT_FREQUENCY,
    "output_voltage": SensorType.OUTPUT_VOLTAGE,
    "output_current": SensorType.OUTPUT_CURRENT,
    "output_power": SensorType.OUTPUT_POWER,
    "temperature": SensorType.TEMPERATURE,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HuaweiR4875G1),
        cv.Required(CONF_CANBUS_ID): cv.use_id(canbus.Canbus),
        cv.Optional(CONF_UPDATE_INTERVAL, default="1s"): cv.update_interval,
        cv.Optional("sensors"): cv.Schema(
            {
                cv.Optional(CONF_INPUT_VOLTAGE): cv.Schema(
                    {
                        cv.Required("multiplier"): cv.float_,
                        cv.Required("offset"): cv.float_,
                    }
                ),
                cv.Optional(CONF_OUTPUT_VOLTAGE): cv.Schema(
                    {
                        cv.Required("multiplier"): cv.float_,
                        cv.Required("offset"): cv.float_,
                    }
                ),
                cv.Optional(CONF_INPUT_CURRENT): cv.Schema(
                    {
                        cv.Required("multiplier"): cv.float_,
                        cv.Required("offset"): cv.float_,
                    }
                ),
                cv.Optional(CONF_OUTPUT_CURRENT): cv.Schema(
                    {
                        cv.Required("multiplier"): cv.float_,
                        cv.Required("offset"): cv.float_,
                    }
                ),
            }
        ),
        cv.Optional("voltage_limits"): cv.Schema(
            {
                cv.Required(CONF_MIN_VOLTAGE): cv.float_,
                cv.Required(CONF_MAX_VOLTAGE): cv.float_,
            }
        ),
        cv.Optional("current_limits"): cv.Schema(
            {
                cv.Required(CONF_MIN_CURRENT): cv.float_,
                cv.Required(CONF_MAX_CURRENT): cv.float_,
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

SENSOR_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HuaweiR4875G1),
        cv.Required("type"): cv.enum(SENSOR_TYPES, lower=True),
    }
).extend(sensor.SENSOR_SCHEMA)

BINARY_SENSOR_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HuaweiR4875G1),
    }
).extend(binary_sensor.BINARY_SENSOR_SCHEMA)

SET_VOLTAGE_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(HuaweiR4875G1),
        cv.Required("voltage"): cv.templatable(cv.float_),
    }
)

SET_CURRENT_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(HuaweiR4875G1),
        cv.Required("current"): cv.templatable(cv.float_),
    }
)

ENABLE_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(HuaweiR4875G1),
    }
)

DISABLE_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(HuaweiR4875G1),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    canbus_ = await cg.get_variable(config[CONF_CANBUS_ID])
    cg.add(var.set_canbus(canbus_))

    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

    sensors_config = config.get("sensors", {})
    if CONF_INPUT_VOLTAGE in sensors_config:
        calib = sensors_config[CONF_INPUT_VOLTAGE]
        cg.add(var.set_input_voltage_calibration(calib["multiplier"], calib["offset"]))
    if CONF_OUTPUT_VOLTAGE in sensors_config:
        calib = sensors_config[CONF_OUTPUT_VOLTAGE]
        cg.add(var.set_output_voltage_calibration(calib["multiplier"], calib["offset"]))
    if CONF_INPUT_CURRENT in sensors_config:
        calib = sensors_config[CONF_INPUT_CURRENT]
        cg.add(var.set_input_current_calibration(calib["multiplier"], calib["offset"]))
    if CONF_OUTPUT_CURRENT in sensors_config:
        calib = sensors_config[CONF_OUTPUT_CURRENT]
        cg.add(var.set_output_current_calibration(calib["multiplier"], calib["offset"]))

    voltage_limits = config.get("voltage_limits", {})
    if voltage_limits:
        cg.add(var.set_voltage_limits(
            voltage_limits[CONF_MIN_VOLTAGE],
            voltage_limits[CONF_MAX_VOLTAGE]
        ))

    current_limits = config.get("current_limits", {})
    if current_limits:
        cg.add(var.set_current_limits(
            current_limits[CONF_MIN_CURRENT],
            current_limits[CONF_MAX_CURRENT]
        ))

@sensor.register_sensor("huawei_r4875g1", SENSOR_SCHEMA)
async def huawei_sensor_to_code(config, sensor_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(sensor_id, template_arg, parent, config["type"])
    await sensor.register_sensor(var, config)
    return var

@binary_sensor.register_binary_sensor("huawei_r4875g1", BINARY_SENSOR_SCHEMA)
async def huawei_binary_sensor_to_code(config, sensor_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(sensor_id, template_arg, parent)
    await binary_sensor.register_binary_sensor(var, config)
    return var

@automation.register_action(
    "huawei_r4875g1.set_voltage",
    huawei_r4875g1_ns.class_("HuaweiR4875G1SetVoltageAction"),
    SET_VOLTAGE_ACTION_SCHEMA,
)
async def set_voltage_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    voltage_ = await cg.templatable(config["voltage"], args, float)
    cg.add(var.set_voltage(voltage_))
    return var

@automation.register_action(
    "huawei_r4875g1.set_current",
    huawei_r4875g1_ns.class_("HuaweiR4875G1SetCurrentAction"),
    SET_CURRENT_ACTION_SCHEMA,
)
async def set_current_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    current_ = await cg.templatable(config["current"], args, float)
    cg.add(var.set_current(current_))
    return var

@automation.register_action(
    "huawei_r4875g1.enable",
    huawei_r4875g1_ns.class_("HuaweiR4875G1EnableAction"),
    ENABLE_ACTION_SCHEMA,
)
async def enable_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    cg.add(var.enable())
    return var

@automation.register_action(
    "huawei_r4875g1.disable",
    huawei_r4875g1_ns.class_("HuaweiR4875G1DisableAction"),
    DISABLE_ACTION_SCHEMA,
)
async def disable_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    cg.add(var.disable())
    return var