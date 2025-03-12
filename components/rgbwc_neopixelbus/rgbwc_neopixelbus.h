#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include <NeoPixelBus.h>

namespace esphome {
namespace rgbwc_neopixelbus {

class NeoPixelRgbwc : public light::LightOutput, public Component {
 public:
  NeoPixelRgbwc(uint16_t pixel_count, uint8_t gpio_pin, uint8_t rmt);
  NeoPixelBus<NeoRgbwcFeature, NeoEsp32RmtNWs2805Method> pixels;
  void setup() override;
  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;
  void dump_config() override;
  

 private:

  bool rgb_mode = true;

  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint16_t temperature_kelvins;
  uint8_t brightness;

  uint8_t last_r;
  uint8_t last_g;
  uint8_t last_b;
  uint16_t last_temperature;
  uint8_t last_brightness;

  RgbwwColor off_state = RgbwwColor(0, 0, 0, 0, 0);
  RgbwwColor current_state;

  uint16_t pixel_count;
  uint8_t gpio_pin;
  NeoBusChannel channel;

  void set_color(RgbwwColor color_state);
  RgbwwColor calculate_rgb_values();
  RgbwwColor calculate_cct_values();

};

} //namespace rgbwc_neopixelbus
} //namespace esphome