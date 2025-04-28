#include "stopwatch.h"
#include "OreonMath.hpp"
#include "hardware/bluetooth.h"
#include "hardware/input.h"
#include "hardware/wifi.h"
#include "ui/icons.h"
#include "ui/status.h"
#include "ui/ui.h"
#include "utils.h"
#include <Fonts/FreeMonoBold12pt7b.h>

using namespace VectorMath;

namespace stopwatch {
void show() {
  bool open = true;
  float time = 0.0;
  bool paused = true;

  using namespace ui::shortcuts;
  ui::Container menu(true);
  menu.size = ui::screenSize();
  menu.alignItems = 0.5;
  menu << status::bar;
  menu << vcenter(
      {
          flabel(
              [&time]() {
                return format("%02u:%04.1f", (int)(time / 60.0f),
                              fmod(time, 60.0f));
              },
              1, WHITE, &FreeMonoBold12pt7b),
          button(flabel(
                     [&time, &paused]() {
                       return time == 0.0 ? "Start"
                                          : (paused ? "Resume" : "Pause");
                     },
                     2),
                 [&paused]() { paused = !paused; }),
          button(flabel(
                     [&time, &paused]() {
                       return time != 0.0 && paused ? "Reset" : "Exit";
                     },
                     2),
                 [&open, &time, &paused]() {
                   if (time != 0.0 && paused) {
                     time = 0.0, paused = true;
                   } else {
                     open = false;
                   }
                 }),
      },
      vec2u(0, ui::screen.height() - 20));

  uint32_t t = millis();
  while (open) {
    input.update();
    wifi::tick();
    bluetooth::silence();
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();
    uint32_t t1 = millis();
    if (!paused)
      time += (t1 - t) / 1000.0;
    t = t1;
  }
}
} // namespace stopwatch