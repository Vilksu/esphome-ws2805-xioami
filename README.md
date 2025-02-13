# ESPHome NeoPixelBus 2.8.3 and Xiaomi Monitor Light Bar components

## rgbwc_neopixelbus component
Basic addressable RGBWC led strip light component for [ESPHome](https://esphome.io/).

Using [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) 2.8.3.

Feature and method: <NeoRgbwcFeature, NeoEsp32Rmt0Ws2805Method>

If you need to change the method just fork the repo and edit `rgbwc_neopixelbus.h`

## Example
```yaml
external_components:
  - source: github://Jak3zz/esphome-ws2805
    components: [rgbwc_neopixelbus]

light:
  - platform: rgbwc_neopixelbus
    name: Ambiance Light
    pin: 16
    num_leds: 10
```

## xiaomi_monitor_light component
Xiaomi monitor light bar remote control component for [ESPHome](https://esphome.io/).

Made for *Mi Computer Monitor Light Bar*

Connect pins to remotes encoders legs marked on the pcb with A, B and D.
Also the remote needs to be powered with 3.3v

![Image of remote pcb](./xiaomi_light_remote.jpg?raw=true)

If the light is out of sync with Esphome: turn on from Esphome and power cycle the light. It will turn on.
## Example
```yaml
external_components:
  - source: github://Jak3zz/esphome-ws2805
    components: [xiaomi_monitor_light]

light:
  - platform: xiaomi_monitor_light
    name: "Xiaomi Light Bar"
    id: light_bar
    pin_a: 33
    pin_b: 25
    pin_d: 32
```
