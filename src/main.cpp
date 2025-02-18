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

  auto center = std::make_shared<ui::Container>(
      true, ui::Container::Alignment::Center, 1.0);
  center->size.y = ui::screen.height(); // - status::bar.size.y;
  {
    auto time = std::make_shared<ui::Container>();
    time->alignItems = 1.0;
    *time << new ui::FunctionalLabel(formatTime1, 2, WHITE,
                                     &FreeMonoBoldOblique9pt7b);
    *time << new ui::FunctionalLabel(formatTime2, 1, WHITE,
                                     &FreeMonoBoldOblique9pt7b);

    *center << std::move(time);
  }
  *center << new ui::FunctionalLabel(formatDate);

  mainScreen << status::bar;
  mainScreen << std::move(center);

  menu::createUI();
}

void loop() {
  input.update();
  ui::screen.fillScreen(BLACK);

  mainScreen.layout(ui::screenSize());
  mainScreen.draw(0);
  ui::show();

  if (input.ok.click()) {
    menu::show();
  }

  if (millis() - input.lastActive > 10000) {
    power::sleep();
  }
}