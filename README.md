# ESPHome NeoPixelBus 2.8.3 component

Basic addressable RGBWC led strip light component for [ESPHome](https://esphome.io/).

Using [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) 2.8.3.

Feature and method: <NeoRgbwcFeature, NeoEsp32Rmt0Ws2805Method>

If you need to change the method just fork the repo and edit `rgbwc_neopixelbus.h`

## Example
```yaml
external_components:
  - source: github.com/Jak3zz/esphome-ws2805
    components: [rgbwc_neopixelbus]

light:
  - platform: rgbwc_neopixelbus
    pin: 16
    num_leds: 10

    name: WS2805 RGBWC Led Strip
    default_transition_length: 500ms
    restore_mode: RESTORE_AND_OFF
```

