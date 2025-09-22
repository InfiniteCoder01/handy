#include <Arduino.h>
#include <WiFi.h>
#include "hardware.hpp"
#include "display.hpp"

static void wifi() {
  while (true) {
    WiFi.scanNetworks(true);
    int8_t cnt;
    while ((cnt = WiFi.scanComplete()) < 0) {
      updateInput();
      if (input.active() && input.a) return;
      screen.fillScreen(0);
      u8g2.setCursor(0, u8g2.getFontAscent());
      ui::println("Scanning...");
      updateDisplay();
    }
    const char *menu[cnt + 1];
    menu[0] = "Refresh";
    if (cnt >= 0) {
      for (auto i = 0; i < cnt; i++) menu[i + 1] = WiFi.SSID(i);
    }

    int opt = ui::select(menu, cnt + 1);
    if (opt < 0) break;
    else if (opt == 0) continue;
    else opt--;

    //
  }
}

static void status() {
  while (true) {
    updateInput();
    if (input.active() && input.a) break;
    screen.fillScreen(0);
    u8g2.setCursor(0, u8g2.getFontAscent());
    ui::printf("Voltage: %0.2f", voltage());
    updateDisplay();
  }
}

void settings() {
  int idx = 0;
  while (true) {
    const char *settings[] = {
      "WiFi",
      "Status",
    };
    idx = ui::select(settings, sizeof(settings) / sizeof(settings[0]), idx);
    if (idx < 0) break;
    else if (idx == 0) wifi();
    else status();
  }
}