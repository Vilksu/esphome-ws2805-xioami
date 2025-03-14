#include "xiaomi_monitor_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace xiaomi_monitor_light {

static const char *TAG = "xiaomi_monitor_light.light";

XiaomiMonitorLight::XiaomiMonitorLight(int pinD, int pinA, int pinB) : pinD(pinD), pinA(pinA), pinB(pinB) {
  pinMode(pinD, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
}

void XiaomiMonitorLight::setup() {}


light::LightTraits XiaomiMonitorLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE, light::ColorMode::BRIGHTNESS});
  traits.set_min_mireds(152); // Minimum color temperature in mired
  traits.set_max_mireds(304); // Maximum color temperature in mired
  return traits;
}

void XiaomiMonitorLight::write_state(light::LightState *state) {
  ESP_LOGCONFIG(TAG, "New state recieved:");
  ESP_LOGCONFIG(TAG, "current_values.is_on(): %f", state->current_values.get_brightness());
  ESP_LOGCONFIG(TAG, "current_values.get_brightness(): %f", state->current_values.get_brightness());
  ESP_LOGCONFIG(TAG, "current_values.get_color_temperature(): %f", state->current_values.get_color_temperature());
}




//Loop function

void XiaomiMonitorLight::StepEncoder(bool dir) {
  //Step encoder in a direction
  ESP_LOGCONFIG(TAG, "Stepping encoder to: %d", dir);

  encoderState = 1;
  if(dir)
  {
    encoderDir = 1;
  }
  else
  {
    encoderDir = -1;
  }
}

void XiaomiMonitorLight::loop() {
  if(millis()-loopTimer < loopInterval) return;
  long debugTimer = millis();

  if(debugDir == 1) {
    StepEncoder(true);
    debugNumber ++;
  }
  else {
    StepEncoder(false);
    debugNumber --;
  }

  if(debugNumber < 10) debugDir = 1;
  if(debugNumber < 1) debugDir = 0;
  



  ESP_LOGCONFIG(TAG, "Loop took %d ms", debugTimer - millis());
  loopTimer = millis();

  encoderLoop();
}

void XiaomiMonitorLight::encoderLoop() {
  if(millis()-encoderTimer < encoderLoopInterval) return;
  if(encoderDir == 0) return;
  if(encoderState == 0) return;
  long debugTimer = millis();

  int FirstPin = pinB;
  int SecondPin = pinA;
  if(encoderDir == 1)
  {
    FirstPin = pinA;
    SecondPin = pinB;
  }
  
  if(encoderState == 1)
  {
    //First rising
    digitalWrite(FirstPin,LOW);
  }
  else if(encoderState == 2)
  {
    //Second rising
    digitalWrite(SecondPin,LOW);
  }
  else if(encoderState == 3)
  {
    //First falling
    digitalWrite(FirstPin,HIGH);
  }
  else if(encoderState == 4)
  {
    //Second falling
    digitalWrite(SecondPin,HIGH);
  }
  encoderState ++;
  if(encoderState == 5)
  {
    encoderState = 0;
    encoderDir = 0;
  }


  ESP_LOGCONFIG(TAG, "Encoder loop took %d ms", debugTimer - millis());
  encoderTimer = millis();
}




void XiaomiMonitorLight::dump_config() { ESP_LOGCONFIG(TAG, "Empty custom light"); }

}  // namespace xiaomi_monitor_light
}  // namespace esphome
