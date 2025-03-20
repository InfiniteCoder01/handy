#include "apps/menu.h"
#include "hardware/bluetooth.h"
#include "hardware/input.h"
#include "hardware/power.h"
#include "hardware/time.h"
#include "hardware/wifi.h"
#include "ui/status.h"
#include "ui/ui.h"
#include <Arduino.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

static ui::Container mainScreen;

void setup() {
  Serial.begin(9600);
  Serial.println("Handy - Best watch in the world!");

  power::init();
  ui::initializeDisplay();
  input.init();

#if defined(HOME_WIFI_SSID) && defined(HOME_WIFI_PASS)
  wifi::knownNetworks[HOME_WIFI_SSID] = HOME_WIFI_PASS;
#endif

  status::createUI();
  mainScreen.size = ui::screenSize();
  mainScreen.justifyContent = 0.5;

  using namespace ui::shortcuts;
  mainScreen << status::bar;
  mainScreen << vcenter(
      {aside({
          baseline(flabel(formatTime1, 2, WHITE, &FreeMonoBoldOblique9pt7b),
                   flabel(formatTime2, 1, WHITE, &FreeMonoBoldOblique9pt7b)),
          flabel(formatDate),
      })},
      vec2u(0, ui::screen.height() - 20));
}

void loop() {
  input.update();
  wifi::tick();
  bluetooth::silence();
  ui::screen.fillScreen(BLACK);
  ui::serve(mainScreen);
  ui::show();

  if (input.right.click()) {
    menu::show();
  }

  if (millis() - input.lastActive > 10000) {
    power::sleep();
  }
}