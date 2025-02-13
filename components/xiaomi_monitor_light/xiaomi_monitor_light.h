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
    void write_state(light::LightState *state) override;
    light::LightTraits get_traits() override;
    void dump_config() override;

private:
    void stepEncoder(bool dir);

    void togglePower();
    void setBrightness(std::function<void()> on_complete = nullptr);  
    void setTemperature(std::function<void()> on_complete = nullptr);

    int lightBarValue = 0;
    int lightBarValueTarget = 0;
    int lightBarTemp = 0;
    int lightBarTempTarget = 0;
    bool lightBarPower = false;
    bool lightBarPowerTarget = false;

    int clickDelay = 100; 

    int encoderDelay = 10; 
    int encoderDir = 0;
    int encoderState = 0;

    int pinD, pinA, pinB;
};

}  // namespace xiaomi_monitor_light
}  // namespace esphome

