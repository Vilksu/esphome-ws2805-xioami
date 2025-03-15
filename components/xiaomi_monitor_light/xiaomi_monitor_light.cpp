#include "xiaomi_monitor_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace xiaomi_monitor_light {

static const char *TAG = "xiaomi_monitor_light.light";

XiaomiMonitorLight::XiaomiMonitorLight(int pinD, int pinA, int pinB) : pinD(pinD), pinA(pinA), pinB(pinB) {
  pinMode(pinD, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);

  digitalWrite(pinD, HIGH);
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
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
  //ESP_LOGCONFIG(TAG, "New state recieved:");
  //ESP_LOGCONFIG(TAG, "current_values.is_on(): %f", state->current_values.get_brightness());
  //ESP_LOGCONFIG(TAG, "current_values.get_brightness(): %f", state->current_values.get_brightness());
  //ESP_LOGCONFIG(TAG, "current_values.get_color_temperature(): %f", state->current_values.get_color_temperature());


  lightBarPowerTarget = state->current_values.is_on();

  // 0-1 -> 0-7
  lightBarValueTarget  = round(state->current_values.get_brightness() * 7 );

  // 152-304 -> 0-8
  lightBarTempTarget  = 8 - round((state->current_values.get_color_temperature() - 152) / 19);
}




//  Step encoder
void XiaomiMonitorLight::stepEncoder(bool dir) {
  //ESP_LOGCONFIG(TAG, "Stepping encoder to: %d", dir);

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
  //Calibration
  if(calibarting){
    calibrate();
  }
  else
  {
    lightBarLoop();
  }
  encoderLoop();
}

//  Calibration function
void XiaomiMonitorLight::calibrate() {
  if(millis()-calibartingTimer < loopInterval) return;
  if(calibartingState < 10){
    stepEncoder(false);
    calibartingState ++;
  }
  else {
    if(lightBarButtonState == false) {
      lightBarButtonState = true;
      digitalWrite(pinD, LOW);
      clickTimer = millis();
    }
    else if(millis()-clickTimer > clickDelay) {
      if(calibartingState < 20) {
        stepEncoder(false);
        calibartingState ++;
      }
      else {
        if(lightBarButtonState == false) {
          if(millis() - clickTimer > clickDelay) {
            calibarting = false;
          }
        }
        else {
          ightBarButtonState = false;
          digitalWrite(pinD, HIGH);
          clickTimer = millis();
        }
      }
    }
  }
  calibartingTimer = millis();
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
        //ESP_LOGCONFIG(TAG, "Power state ok, next ->");
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
          //ESP_LOGCONFIG(TAG, "Power state toggled");
        }
      }
      break;

    // Adjusting brightness
    case 1:
      if(!lightBarPower) {
        lightBarState ++; //Adjust temperature only if power is on
        
      }
      else {
        if(lightBarValue == lightBarValueTarget) {
          //ESP_LOGCONFIG(TAG, "Brightness ok, next ->");
          lightBarState ++;
        }
        else {
          //ESP_LOGCONFIG(TAG, "Current brightness: %d, Target brightness: %d", lightBarValue, lightBarValueTarget);
          // Adjust brightness
          //ESP_LOGCONFIG(TAG, "Adjusting brightness");
          if(lightBarValue > lightBarValueTarget) {
            stepEncoder(false);
            lightBarValue --;
          }
          else if(lightBarValue < lightBarValueTarget) {
            stepEncoder(true);
            lightBarValue ++;
          }
        }
      }
      break;
    
    // Adjusting temperature
    case 2:
      if(!lightBarPower) {
        lightBarState ++; //Adjust temperature only if power is on
      }
      else {
        if(lightBarTemp == lightBarTempTarget) {
          // If temperature is ok, let go of the button and move to the next state
          if(lightBarButtonState == false) {
            if(millis() - clickTimer > clickDelay) {
              //ESP_LOGCONFIG(TAG, "Temperature ok, next ->");
              lightBarState ++;
            }
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
            //ESP_LOGCONFIG(TAG, "Adjusting temperature");
            //ESP_LOGCONFIG(TAG, "Current temperature: %d, Target temperature: %d", lightBarTemp, lightBarTempTarget);
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
