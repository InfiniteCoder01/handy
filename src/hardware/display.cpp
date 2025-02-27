#include "display.h"

const uint8_t BACKLIGHT = 16;

namespace ui {
Adafruit_ST7735 tft = Adafruit_ST7735(17, 20, 21);
GFXcanvas16 screen(160, 128);
uint8_t brightness = 255;

void initializeDisplay() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, brightness);
}

void displayPower(bool enabled) {
  if (enabled) {
    tft.enableSleep(false);
    analogWrite(BACKLIGHT, brightness);
  } else {
    digitalWrite(BACKLIGHT, LOW);
    tft.enableSleep(true);
  }
}

void show() {
  tft.startWrite();
  tft.setAddrWindow(0, 0, screen.width(), screen.height());
  tft.writePixels(screen.getBuffer(), screen.width() * screen.height());
  tft.endWrite();
  analogWrite(BACKLIGHT, brightness);
}

void drawImage(vec2i pos, vec2u size, const uint16_t *image) {
  for (uint16_t y = 0; y < size.y; y++) {
    for (uint16_t x = 0; x < size.x; x++) {
      uint16_t color = image[x + y * size.x];
      if (color != MAGENTA) screen.drawPixel(pos.x + x, pos.y + y, color);
    }
  }
}
}