#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include <NeoPixelBus.h>

namespace esphome {
namespace xiaomi_monitor_light {

class XiaomiMonitorLight : public light::LightOutput, public Component {
public:
    XiaomiMonitorLight(int pinD, int pinA, int pinB);

    void setup() override;
    void loop() override;
    void write_state(light::LightState *state) override;
    light::LightTraits get_traits() override;
    void dump_config() override;

private:
    //void stepEncoder(bool dir);
    void lightBarLoop();
    void encoderLoop();
    void stepEncoder(bool dir);
    void calibrate();

    //void togglePower();
    //void setBrightness(std::function<void()> on_complete = nullptr);  
    //void setTemperature(std::function<void()> on_complete = nullptr);

    int lightBarValue = 0;
    int lightBarValueTarget = 0;
    int lightBarTemp = 0;
    int lightBarTempTarget = 0;
    bool lightBarPower = true;
    bool lightBarPowerTarget = true;

    bool calibarting = true;
    int calibartingState = 0;

    int loopInterval = 100;
    int encoderLoopInterval = 12; 
    int clickDelay = 100; 
    
    int lightBarState = 0;
    bool lightBarButtonState = false;

    int encoderDir = 0;
    int encoderState = 0;
    

    int pinD, pinA, pinB;

    long loopTimer = 0;
    long encoderTimer = 0;
    long clickTimer = 0;
    long calibartingTimer = 0;
};

}  // namespace xiaomi_monitor_light
}  // namespace esphome

