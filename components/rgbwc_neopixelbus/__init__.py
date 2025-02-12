import esphome.codegen as cg

rgbwc_neopixelbus_ns = cg.esphome_ns.namespace("rgbwc_neopixelbus")

cg.add_library(
    name="NeoPixelBus",
    repository="https://github.com/Makuna/NeoPixelBus.git",
    version="^2.8.3", 
)
