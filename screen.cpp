#include "display.hpp"
#include <Adafruit_ST7735.h>
#include <LittleFS.h>

uint8_t brightness = 255;
GFXcanvas16 screen(160, 128);
U8G2_FOR_ADAFRUIT_GFX u8g2;

static const uint8_t POWER = 14;
static const uint8_t BACKLIGHT = 15;
static Adafruit_ST7735 tft = Adafruit_ST7735(12, 20, 21);

void setupDisplay() {
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  pinMode(BACKLIGHT, OUTPUT);
  u8g2.begin(screen);
}

void updateDisplay() {
  tft.startWrite();
  tft.setAddrWindow(0, 0, screen.width(), screen.height());
  tft.writePixels(screen.getBuffer(), screen.width() * screen.height());
  tft.endWrite();
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

uint16_t *loadImage(const char *filename) {
  auto file = LittleFS.open(filename, "r");
  file.seek(18);
  const uint8_t width = file.read();
  file.seek(22);
  const uint8_t height = file.read();
  const size_t size = (size_t)width * height;
  uint16_t *buffer = new uint16_t[size];
  file.seek(file.size() - size * 2);
  file.readBytes((char *)buffer, size * 2);
  return buffer;
}