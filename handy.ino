#include "ui.h"
#include "input.h"
#include "power.h"
#include "time.h"
#include "utils.h"
#include <memory>

void setup() {
  Serial.begin(9600);
  ui::initializeDisplay();
  input.init();
  power::init();
}

void loop() {
  input.update();
  ui::screen.fillScreen(BLACK);

  ui::Container layout;
  layout.vertical = true;
  layout.children.push_back(std::make_unique<ui::Label>(format("%02d:%02d:%02d", now.hour(), now.minute(), now.second())));
  layout.children.push_back(std::make_unique<ui::Label>(format("%f% (%fV)", input.percentage, input.voltage)));
  layout.layout(ui::Frame::fullscreen());
  layout.draw(false);

  ui::show();
  if (millis() - input.lastActive > 10000) {
    power::sleep();
  }
}