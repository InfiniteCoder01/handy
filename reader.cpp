#include <Arduino.h>
#include <LittleFS.h>
#include <vector>
#include "hardware.hpp"
#include "display.hpp"

static std::vector<const char *> lineOffsets;
static uint8_t chr_callback(int16_t x, int16_t y, uint16_t e, const char *chr) {
  if (x == 0 && *lineOffsets.rbegin() < chr) lineOffsets.push_back(chr);
  if (y - u8g2.getFontDescent() >= screen.height()) return 2;
  return 0;
}

uint8_t pageReader(String text) {
  lineOffsets = { text.c_str() };
  size_t scroll = 0;
  while (true) {
    delay(20);
    updateInput(true);
    if (input.active()) {
      if (input.a) break;
      if (input.d) return 1;
      int8_t dir = input.c - input.b;
      if (dir < 0 && scroll > 0) scroll--;
      else if (dir > 0 && scroll + 1 < lineOffsets.size()) scroll++;
    }

    Serial.println("Before display");
    screen.fillScreen(0);
    ui::bar();
    u8g2.setCursor(0, u8g2.getFontAscent() + ui::BAR_HEIGHT);
    ui::print(lineOffsets[scroll], chr_callback);
    updateDisplay();
  }
  return 0;
}

void reader() {
  // Load pages
  std::vector<String> pages;
  {
    Dir dir = LittleFS.openDir("book");
    while (dir.next()) pages.push_back(dir.fileName().substring(0, dir.fileName().length() - 3));
  }

  const char *pages_cstr[pages.size()];
  for (size_t i = 0; i < pages.size(); i++) pages_cstr[i] = pages[i].c_str();

  // Main loop
  int page = 0;
  bool selectPage = true;
  while (true) {
    if (selectPage) page = ui::select(pages_cstr, pages.size(), page);
    else selectPage = true;

    if (page < 0) return;
    if (page >= pages.size()) continue;
    File file = LittleFS.open(format("book/%s.md", pages_cstr[page]).c_str(), "r");
    String text = file.readString();
    file.close();

    const uint8_t cont = pageReader(text);
    if (cont == 1) {
      page++;
      selectPage = false;
    }
  }
}