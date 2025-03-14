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


  lightBarPowerTarget = state->current_values.is_on();
  lightBarValueTarget = state->current_values.get_brightness();
  lightBarTempTarget = state->current_values.get_color_temperature();
}




//  Step encoder
void XiaomiMonitorLight::StepEncoder(bool dir) {
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

//  Main loop
void XiaomiMonitorLight::loop() {
  lightbarLoop();
  encoderLoop();
}

//  Lightbar loop
void XiaomiMonitorLight::lightbarLoop() {
  if(millis()-loopTimer < loopInterval) return;
  long debugTimer = millis();


  switch (lightbarState) {
    case 0:
      if(lightBarPower == lightBarPowerTarget)
      {
        ESP_LOGCONFIG(TAG, "Power state ok, next ->");
        lightbarState ++;
      }
      else
      {
        if(lightbarButtonState == false)
        {
          lightbarButtonState = true;
          digitalWrite(pinD, LOW);
          clickTimer = millis();
        }
        else if(millis()-clickTimer > clickDelay)
        {
          lightbarButtonState = false;
          digitalWrite(pinD, HIGH);
          lightBarPower = lightBarPowerTarget;
          ESP_LOGCONFIG(TAG, "Power state toggled");
        }
      }
      
  }
  



  //ESP_LOGCONFIG(TAG, "Loop took %d ms", millis()-debugTimer);
  loopTimer = millis();
}

//  Encoder loop
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

  encoderTimer = millis();
}




void XiaomiMonitorLight::dump_config() { ESP_LOGCONFIG(TAG, "Empty custom light"); }

}  // namespace xiaomi_monitor_light
}  // namespace esphome
