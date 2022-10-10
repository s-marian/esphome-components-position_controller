import esphome.codegen as cg
import esphome.config_validation as cv
#from esphome.components import web_server_base
#from esphome.components.web_server_base import CONF_WEB_SERVER_BASE_ID
from esphome.components import sensor, output
from esphome.const import (
  CONF_ID,
  CONF_PIN_A,
  CONF_PIN_B,
)
from esphome.core import coroutine_with_priority, CORE

#AUTO_LOAD = ["web_server_base"]
#DEPENDENCIES = ["wifi"]
CODEOWNERS = ["@s-marian"]


CONF_ACTUAL_POSITION="actual_position"
CONF_TARGET_POSITION="target_position"
CONF_ACCEL_UPDATE_FREQ="accel_update_frequency"
CONF_ADJUSTMENT_THRESHOLD="adjustment_threshold"
CONF_TARGET_POSITION_OFFSET="target_position_offset"
CONF_ROTATION_OFFSET="rotation_offset"

position_controller_ns = cg.esphome_ns.namespace("position_controller")
PositionController = position_controller_ns.class_("PositionController", cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PositionController),
            cv.Required(CONF_ACTUAL_POSITION): cv.use_id(sensor.Sensor),
            cv.Required(CONF_TARGET_POSITION): cv.use_id(sensor.Sensor),
            cv.Required(CONF_ACCEL_UPDATE_FREQ): cv.frequency,
            cv.Required(CONF_ADJUSTMENT_THRESHOLD): cv.angle,
            cv.Required(CONF_PIN_A): cv.use_id(output.FloatOutput),
            cv.Required(CONF_PIN_B): cv.use_id(output.FloatOutput),
            cv.Optional(CONF_TARGET_POSITION_OFFSET): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_ROTATION_OFFSET): cv.angle,
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


#@coroutine_with_priority(64.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    act_pos = await cg.get_variable(config[CONF_ACTUAL_POSITION])
    tgt_pos = await cg.get_variable(config[CONF_TARGET_POSITION])
    cg.add(var.set_actual_position_sensor(act_pos))
    cg.add(var.set_target_position_sensor(tgt_pos))
    cg.add(var.set_accel_update_freq(config[CONF_ACCEL_UPDATE_FREQ]))
    cg.add(var.set_adjustment_threshold(config[CONF_ADJUSTMENT_THRESHOLD]))
    pin_a_ = await cg.get_variable(config[CONF_PIN_A])
    cg.add(var.set_pin_a(pin_a_))
    pin_b_ = await cg.get_variable(config[CONF_PIN_B])
    cg.add(var.set_pin_b(pin_b_))
    if CONF_TARGET_POSITION_OFFSET in config:
        target_position_offset = await cg.get_variable(config[CONF_TARGET_POSITION_OFFSET])
        cg.add(var.set_target_position_offset_sensor(target_position_offset))

    if CONF_ROTATION_OFFSET in config:
        cg.add(var.set_rotation_offset(config[CONF_ROTATION_OFFSET]))
