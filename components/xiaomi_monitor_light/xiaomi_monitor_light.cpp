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
void XiaomiMonitorLight::stepEncoder(bool dir) {
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
  lightBarLoop();
  encoderLoop();
}

//  Lightbar loop
void XiaomiMonitorLight::lightBarLoop() {
  if(millis()-loopTimer < loopInterval) return;
  long debugTimer = millis();


  switch (lightBarState) {
    // Adjusting power
    case 0:
      if(lightBarPower == lightBarPowerTarget)
      {
        ESP_LOGCONFIG(TAG, "Power state ok, next ->");
        lightBarState ++;
      }
      else
      {
        if(lightBarButtonState == false)
        {
          lightBarButtonState = true;
          digitalWrite(pinD, LOW);
          clickTimer = millis();
        }
        else if(millis()-clickTimer > clickDelay)
        {
          lightBarButtonState = false;
          digitalWrite(pinD, HIGH);
          lightBarPower = lightBarPowerTarget;
          ESP_LOGCONFIG(TAG, "Power state toggled");
        }
      }
      break;

    // Adjusting brightness
    case 1:
      if(!lightBarPower) lightBarState ++; //Adjust temperature only if power is on
      else {
        if(lightBarValue > lightBarValueTarget) {
          stepEncoder(false);
          lightBarValue --;
        }
        else if(lightBarValue < lightBarValueTarget) {
          stepEncoder(true);
          lightBarValue ++;
        }
      }
      break;
    
    // Adjusting temperature
    case 2:
      if(!lightBarPower) lightBarState ++; //Adjust temperature only if power is on
      else {
        if(lightBarTemp == lightBarTempTarget) {
          // If temperature is ok, let go of the button and move to the next state
          if(lightBarButtonState == false) {
            if(millis() - clickTimer > clickDelay) lightBarState ++;
          }
          else {
            // Let go of the button
            lightBarButtonState = false;
            digitalWrite(pinD, HIGH);
            clickTimer = millis();
          }
        }
        else {
          // Temperature is not ok, keep adjusting
          if(lightBarButtonState == false) {
            // Press down the button
            lightBarButtonState = true;
            digitalWrite(pinD, LOW);
            clickTimer = millis();
          }
          else if(millis() - clickTimer > clickDelay) {
            // Adjust temperature
            if(lightBarTemp > lightBarTempTarget) {
              stepEncoder(false);
              lightBarTemp --;
            }
            else if(lightBarTemp < lightBarTempTarget) {
              stepEncoder(true);
              lightBarTemp ++;
            }
          }
        }
      }
      break;

    default:
      lightBarState = 0;
      break;  
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
