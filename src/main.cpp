#include "apps/menu.h"
#include "input.h"
#include "power.h"
#include "ui/status.h"
#include "ui/ui.h"
#include <Arduino.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

static ui::Container mainScreen;

void setup() {
  Serial.begin(9600);
  Serial.println("Handy - Best watch in the world!");

  power::init();
  ui::initializeDisplay();
  input.init();

  status::createUI();
  mainScreen.size = ui::screenSize();
  mainScreen.justifyContent = ui::Container::Alignment::Center;

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
  ui::screen.fillScreen(BLACK);

  mainScreen.layout(ui::screenSize());
  mainScreen.draw(0, true);
  ui::show();

  if (input.right.click()) {
    menu::show();
  }

  if (millis() - input.lastActive > 10000) {
    power::sleep();
  }
}