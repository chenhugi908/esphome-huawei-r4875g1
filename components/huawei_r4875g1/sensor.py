from . import huawei_sensor_to_code, SENSOR_SCHEMA

import esphome.components.sensor as sensor

CONFIG_SCHEMA = SENSOR_SCHEMA

async def to_code(config):
    return await huawei_sensor_to_code(config, config[CONF_ID], None, None)