#include <Arduino.h>
#include "hardware.hpp"
#include "display.hpp"

void vault() {
  while (true) {
    updateInput();
    if (input.active() && input.a) break;

    screen.fillScreen(0x0000);
    u8g2.setCursor(0, u8g2.getFontAscent());
    ui::println("Nothing to see here right now.");
    updateDisplay();
  }
}