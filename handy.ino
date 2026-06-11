#include "hardware.hpp"
#include "display.hpp"
#include "data/secrets.h"
#include <LittleFS.h>

static uint16_t *background;
void setup() {
  Serial.begin();
  LittleFS.begin();
  initRTC();
  setupInput();
  setupDisplay();
  addWifiNetworks();

  background = loadImage("background.bmp");
}

void reader();
void vault();
void settings();

void loop() {
  updateInput(true);
  if (input.active() && input.d) {
    const char *apps[] = {
      "Reader",
      "Vault",
      "Settings",
    };
    ui::defaultFont();
    int app = 0;
    while (true) {
      app = ui::select(apps, sizeof(apps) / sizeof(apps[0]), app);
      if (app < 0) break;
      else if (app == 0) reader();
      else if (app == 1) vault();
      else settings();
    }
    updateRTC();
  }

  // screen.fillScreen(0);
  screen.drawRGBBitmap(0, 0, background, screen.width(), screen.height());

  // Time
  const String time1 = formatTime1(), time2 = formatTime2();

  u8g2.setFont(u8g2_font_profont29_tf);
  int16_t width = u8g2.getUTF8Width(time1.c_str());
  u8g2.setFont(u8g2_font_DigitalDisco_tf);
  width += u8g2.getUTF8Width(time2.c_str());

  u8g2.setCursor((screen.width() - width) / 2, screen.height() / 2);
  u8g2.setFont(u8g2_font_profont29_tf);
  u8g2.setFontMode(1);
  u8g2.print(formatTime1());
  u8g2.setFont(u8g2_font_DigitalDisco_tf);
  u8g2.setFontMode(1);
  u8g2.println(formatTime2());

  // Date
  ui::defaultFont();
  const String date = formatDate();
  width = u8g2.getUTF8Width(date.c_str());

  u8g2.tx = (screen.width() - width) / 2;
  u8g2.print(date);

  ui::bar();
  updateDisplay();
}
