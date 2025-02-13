import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import light
from esphome.const import (
    CONF_OUTPUT_ID,
    CONF_DEFAULT_TRANSITION_LENGTH,
)

CONF_PIN_A = "pin_a"
CONF_PIN_B = "pin_b"
CONF_PIN_D = "pin_d"

xiaomi_monitor_light_ns = cg.esphome_ns.namespace("xiaomi_monitor_light")
XiaomiMonitorLight = xiaomi_monitor_light_ns.class_("XiaomiMonitorLight", light.LightOutput, cg.Component)


CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(XiaomiMonitorLight),
        cv.Required(CONF_PIN_A): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_PIN_B): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_PIN_D): pins.internal_gpio_output_pin_number,
        cv.Optional(CONF_DEFAULT_TRANSITION_LENGTH, default="0s"): cv.positive_time_period_milliseconds,
    }   
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], config[CONF_PIN_D], config[CONF_PIN_A], config[CONF_PIN_B])
    await cg.register_component(var, config)
    await light.register_light(var, config)

