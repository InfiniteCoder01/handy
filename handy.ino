#include "hardware.hpp"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

const uint8_t BACKLIGHT = 16;
Adafruit_ST7735 tft = Adafruit_ST7735(17, 20, 21);
GFXcanvas16 screen(160, 128);

void setup() {
  Serial.begin(115200);
  initRTC();

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  pinMode(BACKLIGHT, OUTPUT);
  analogWrite(BACKLIGHT, 128);
}

void loop() {
  updateRTC();
  tft.setCursor(0, 0);
  tft.print(formatTime());
  delay(500);
}
