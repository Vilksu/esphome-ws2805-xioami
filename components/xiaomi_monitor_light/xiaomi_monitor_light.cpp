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
  // if (state->current_values.is_on()) {
    
  //   lightBarPowerTarget = true;
  //   // brightness 0-7
  //   lightBarValueTarget = round(state->current_values.get_brightness() * 7 ); 
  //   // scale between 152-304 to 0-8
  //   lightBarTempTarget = 8 - round((state->current_values.get_color_temperature() - 152) / 19);
  // } else {
  //   lightBarPowerTarget = false;
  // }

  // if (lightBarPower != lightBarPowerTarget) {
  //   togglePower();
  // } else if (lightBarValue != lightBarValueTarget) {
  //   setBrightness([this]() {  // Adjust temperature after brightness is done
  //     if (lightBarTemp != lightBarTempTarget) {
  //       digitalWrite(pinD, LOW);
  //       setTemperature([this]() {
  //         digitalWrite(pinD, HIGH);  // Set pinD HIGH after temperature adjustment is complete
  //       });
  //     }
  //   });
  // } else if (lightBarTemp != lightBarTempTarget) {
  //   digitalWrite(pinD, LOW);
  //   setTemperature([this]() {
  //     digitalWrite(pinD, HIGH);  // Ensure pinD is HIGH after temperature adjustment
  //   });
  // }
}

void XiaomiMonitorLight::togglePower() {
  ESP_LOGCONFIG(TAG, "Toggle power");

  // digitalWrite(pinD, LOW);
  // this->set_timeout("toggle_power", clickDelay, [this]() {
  //   digitalWrite(pinD, HIGH);
  //   lightBarPower = !lightBarPower;
  //   if (lightBarValue != lightBarValueTarget) {
  //     setBrightness([this]() {
  //       if (lightBarTemp != lightBarTempTarget) {
  //         digitalWrite(pinD, LOW);
  //         setTemperature([this]() {
  //           digitalWrite(pinD, HIGH);
  //         });
  //       }
  //     });
  //   }
  // });
}

void XiaomiMonitorLight::setBrightness(std::function<void()> on_complete) {
  if (lightBarValue == lightBarValueTarget) {  
    if (on_complete) on_complete();  // Call callback if adjustment is already done
    return;
  }

  bool increase = (lightBarValue < lightBarValueTarget);

  this->set_timeout("brightness_step", 4 * encoderDelay, [this, increase, on_complete]() {
    if (increase) {
      stepEncoder(true);
      lightBarValue++;
    } else {
      stepEncoder(false);
      lightBarValue--;
    }

    if (lightBarValue == lightBarValueTarget) {
      if (on_complete) on_complete();  // Call callback when brightness is set
    } else {
      setBrightness(on_complete);  // Continue adjusting
    }
  });
}

void XiaomiMonitorLight::setTemperature(std::function<void()> on_complete) {
  if (lightBarTemp == lightBarTempTarget) {  
    if (on_complete) on_complete();  // Call callback immediately if adjustment is done
    return;
  }

  bool increase = (lightBarTemp < lightBarTempTarget);

  this->set_timeout("temperature_step", 4 * encoderDelay, [this, increase, on_complete]() {
    if (increase) {
      stepEncoder(true);
      lightBarTemp++;
    } else {
      stepEncoder(false);
      lightBarTemp--;
    }

    if (lightBarTemp == lightBarTempTarget) {
      if (on_complete) on_complete();  // Call callback when adjustment is complete
    } else {
      setTemperature(on_complete);  // Continue adjusting
    }
  });
}

void XiaomiMonitorLight::stepEncoder(bool dir) {
  encoderState = 1;
  encoderDir = dir ? 1 : -1;

  int firstPin = (encoderDir == 1) ? pinA : pinB;
  int secondPin = (encoderDir == 1) ? pinB : pinA;

  digitalWrite(firstPin, LOW);
  this->set_timeout("encoder_step_1", encoderDelay, [this, firstPin, secondPin]() {
    digitalWrite(secondPin, LOW);
    this->set_timeout("encoder_step_2", encoderDelay, [this, firstPin, secondPin]() {
      digitalWrite(firstPin, HIGH);
      this->set_timeout("encoder_step_3", encoderDelay, [this, secondPin]() {
        digitalWrite(secondPin, HIGH);
        encoderState = 0;
        encoderDir = 0;
      });
    });
  });
}
///Jarkon koodi loppuu




//Loop function
void XiaomiMonitorLight::loop() {
  if(millis()-loopTimer < loopInterval) return;
  long debugTimer = millis();





  ESP_LOGCONFIG(TAG, "Loop took %d ms", debugTimer - millis());
  loopTimer = millis();
}

void XiaomiMonitorLight:encoderLoop() {
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
