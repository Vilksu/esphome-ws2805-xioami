import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import light
from esphome.const import (
    CONF_NUM_LEDS,
    CONF_PIN,
    CONF_OUTPUT_ID,
)

CONF_RMT = "rmt_channel"
rgbwc_neopixelbus_ns = cg.esphome_ns.namespace("rgbwc_neopixelbus")
NeoPixelRgbwc = rgbwc_neopixelbus_ns.class_("NeoPixelRgbwc", light.LightOutput)


CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(NeoPixelRgbwc),
        cv.Required(CONF_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_NUM_LEDS): cv.positive_not_null_int,
        cv.Optional(CONF_RMT, default=0): cv.int_range(0, 7),
    }   
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], config[CONF_NUM_LEDS], config[CONF_PIN], config[CONF_RMT])
    await light.register_light(var, config)

