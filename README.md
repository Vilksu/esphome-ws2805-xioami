# ESPHome NeoPixelBus 2.8.3 component

Basic addressable RGBWC led strip light component for esphome.
<NeoRgbwcFeature, NeoEsp32Rmt0Ws2805Method>
If you need to change the method just fork the component and edit `rgbwc_neopixelbus.h`
## Example
```yaml
external_components:
  - source: my_components
    components: [rgbwc_neopixelbus]

light:
  - platform: rgbwc_neopixelbus
    pin: 16
    num_leds: 10

    name: WS2805 RGBWC Led Strip
    default_transition_length: 500ms
    restore_mode: RESTORE_AND_OFF
```

