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
    void encoderLoop();

    //void togglePower();
    //void setBrightness(std::function<void()> on_complete = nullptr);  
    //void setTemperature(std::function<void()> on_complete = nullptr);

    int lightBarValue = 0;
    int lightBarValueTarget = 0;
    int lightBarTemp = 0;
    int lightBarTempTarget = 0;
    bool lightBarPower = false;
    bool lightBarPowerTarget = false;

    int loopInterval = 1000;
    int encoderLoopInterval = 10; 
    int clickDelay = 100; 
    

    int encoderDir = 0;
    int encoderState = 0;

    int pinD, pinA, pinB;

    long loopTimer = 0;
    long encoderTimer = 0;

    int debugNumber = 0;
    int debugDir = 1;
};

}  // namespace xiaomi_monitor_light
}  // namespace esphome

