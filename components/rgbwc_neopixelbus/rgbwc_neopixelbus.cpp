#include "rgbwc_neopixelbus.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rgbwc_neopixelbus {

static const char *TAG = "rgbwc_neopixelbus.light";

NeoPixelRgbwc::NeoPixelRgbwc(uint16_t pixel_count, uint8_t gpio_pin, uint8_t rmt)
  : pixel_count(pixel_count),
    gpio_pin(gpio_pin),
    channel(static_cast<NeoBusChannel>(rmt)),
    pixels(pixel_count, gpio_pin, channel)
    
{
  ESP_LOGCONFIG(TAG, "Setting up NeoPixelBus on GPIO %d", gpio_pin);
  pixels.Begin();
}


void NeoPixelRgbwc::setup() {}

light::LightTraits NeoPixelRgbwc::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE ,light::ColorMode::RGB , light::ColorMode::BRIGHTNESS});
  traits.set_min_mireds(153.846);
  traits.set_max_mireds(333.333);
  return traits;
}

void NeoPixelRgbwc::write_state(light::LightState *state) {
  if(!state->current_values.is_on()){
    set_color(off_state);        
  }
  else {
    // Get values from HA
    brightness = static_cast<uint8_t>(state->current_values.get_brightness()*100);
    r = static_cast<uint8_t>(state->current_values.get_red()*255);
    g = static_cast<uint8_t>(state->current_values.get_green()*255);
    b = static_cast<uint8_t>(state->current_values.get_blue()*255);
    temperature_kelvins = static_cast<uint16_t>(state->current_values.get_color_temperature_kelvin());
    
    //ESP_LOGCONFIG(TAG, "RGB: (%d, %d, %d) CCT: (%d), Brightness: %d", r, g, b, temperature_kelvins, brightness);

    bool rgbChanged = (r != last_r || g != last_g || b != last_b);
    bool temperatureChanged = (temperature_kelvins != last_temperature);
    bool brightnessChanged = (brightness != last_brightness);

    // Dont do anything if nothing has changed
    if (rgbChanged || temperatureChanged || brightnessChanged) {
      // Update RGB mode based on color change
      if (rgbChanged) {
          rgb_mode = true;
          current_state = calculate_rgb_values();
          set_color(current_state);
      } 
      else if (temperatureChanged) 
      {
          rgb_mode = false;
          current_state = calculate_cct_values();
          set_color(current_state);
      }
      // Handle brightness change
      else if (brightnessChanged) {
          if (rgb_mode) {
              current_state = calculate_rgb_values();
          } 
          else {
              current_state = calculate_cct_values();
          }
          set_color(current_state);
      }  
      last_r = r;
      last_g = g;
      last_b = b;
      last_temperature = temperature_kelvins;
      last_brightness = brightness;
    }
  }
}

RgbwwColor NeoPixelRgbwc::calculate_rgb_values() {
    // Calculate scaled brightness once
    uint16_t scaled_brightness = (brightness * 256) / 100;

    // Scale r, g, b based on the brightness
    uint8_t scaled_r = (r * scaled_brightness) >> 8;
    uint8_t scaled_g = (g * scaled_brightness) >> 8;
    uint8_t scaled_b = (b * scaled_brightness) >> 8;

    // Return Rgbww color object
    return RgbwwColor(scaled_g, scaled_r, scaled_b, 0, 0);
}

RgbwwColor NeoPixelRgbwc::calculate_cct_values() {
    // Calculate scaled brightness once
    uint16_t scaled_brightness = (brightness * 256) / 100;

    // Initialize CW and WW values
    uint8_t cw = 0;
    uint8_t ww = 0;

    // Calculate CW based on temperature in Kelvin (scaled to 255 range)
    cw = static_cast<uint8_t>((static_cast<float>(temperature_kelvins - 3000) / (6499 - 3000)) * 255);
    ww = 255 - cw;  // WW is the inverse of CW

    // Scale cw and ww (using right shift for efficiency)
    uint8_t scaled_cw = (cw * scaled_brightness) >> 8;
    uint8_t scaled_ww = (ww * scaled_brightness) >> 8;

    // Return the Rgbww color object with the calculated values
    return RgbwwColor(0, 0, 0, scaled_ww, scaled_cw);
}

void NeoPixelRgbwc::dump_config() { ESP_LOGCONFIG(TAG, "Empty custom light"); }

void NeoPixelRgbwc::set_color(RgbwwColor color_state) {
    for (int i = 0; i < pixel_count; i++) {
        pixels.SetPixelColor(i, color_state);
    }
    pixels.Show();
}

}  // namespace rgbwc_neopixelbus
}  // namespace esphome
