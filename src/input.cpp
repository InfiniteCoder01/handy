#include "input.h"
#include "OreonMath.hpp"
#include "power.h"
#include "time.h"

using namespace VectorMath;

Input input;

void Input::init() { initRTC(); }

void Input::update() {
  bool active = false;
  active |= left.tick();
  active |= right.tick();
  active |= up.tick();
  active |= down.tick();
  active |= ok.tick();

  if (active) {
    lastActive = millis();
  }

  {
    static uint32_t t = millis();
    if (millis() - t > 300) {
      const float BAT_LOW = 3.5, BAT_HIGH = 4.05;
      voltage =
          power::voltage() + 0.3; // Add compensation for diode voltage drop
      percentage =
          Math::clamp((voltage - BAT_LOW) / (BAT_HIGH - BAT_LOW), 0.0, 1.0);
      charging = power::charging();
      t = millis();
    }
  }

  {
    static uint32_t t = millis();
    if (millis() - t > 100) {
      updateRTC();
      t = millis();
    }
  }
}
