#include <U8g2_for_Adafruit_GFX.h>
#include <WiFi.h>
#include "hardware.hpp"
#include "display.hpp"

namespace ui {
void bar() {
  updateRTC();
  updateWiFi();

  for (uint8_t y = 1; y <= BAR_HEIGHT - 1; y++) {
    const int16_t radius = 3;
    const int16_t dy = radius - min(min(y - 1, BAR_HEIGHT - 1 - y), radius);
    const uint8_t skip = radius - sqrt(radius * radius - dy * dy);
    for (uint8_t x = 1 + skip; x < screen.width() - 1 - skip; x++) {
      darken(x, y);
    }
  }

  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.setFontMode(1);
  const uint8_t iconsY = (BAR_HEIGHT + u8g2.getFontAscent()) / 2 + 1;
  const uint8_t iconW = u8g2.u8g2.font_info.max_char_width;

  uint8_t barRight;
  // Battery
  if (charging()) {
    barRight = screen.width() - iconW - 1;
    u8g2.drawGlyph(barRight, iconsY, 0xe20e);
  } else {
    const float BAT_LOW = 3.5, BAT_HIGH = 4.05;
    float v = voltage() + 0.3;  // Add compensation for diode voltage drop
    float percentage = constrain((v - BAT_LOW) / (BAT_HIGH - BAT_LOW), 0.0, 1.0);
    
    const uint8_t width = 8;
    const uint8_t height = 4;
    const uint8_t x = screen.width() - width - 7;
    const uint8_t y = (BAR_HEIGHT - height) / 2;
    barRight = x - 3;
    screen.drawRect(x - 2, y - 2, width + 4, height + 4, 0xffff);
    screen.fillRect(x + width + 2, y + height / 2 - 1, 1, 2, 0xffff);
    
    uint16_t r = (uint16_t)min(round((1.0 - percentage) * 62), 31) << 11;
    uint16_t g = (uint16_t)min(round(percentage * 126.0), 61) << 5;
    screen.fillRect(x, y, width * percentage, height, r | g);
  }
  {
    uint16_t icon = 0xe217;
    if (WiFi.isConnected()) icon = 0xe217 + round(constrain(map(WiFi.RSSI(), -100, -30, 0, 100) / 100.0, 0.0, 1.0) * 3);
    u8g2.drawGlyph(barRight - iconW, iconsY, icon);
  }

  defaultFont();
  u8g2.setCursor(2, (BAR_HEIGHT + u8g2.getFontAscent()) / 2);
  u8g2.print(formatTime().c_str());
}

void darken(uint8_t x, uint8_t y) {
  uint16_t color = screen.getPixel(x, y);
  uint8_t r = (color >> 11) & 0x001f;
  uint8_t g = (color >> 5) & 0x003f;
  uint8_t b = color & 0x001f;
  r >>= 1, g >>= 1, b >>= 1;
  color = ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
  screen.drawPixel(x, y, color);
}

void defaultFont() {
  u8g2.setForegroundColor(0xffff);
  u8g2.setFont(u8g2_font_6x12_t_cyrillic);
  u8g2.setFontMode(1);
}

static void control(const String block) {
  if (block == "0") defaultFont();
}

void print(const char *str, uint8_t (*callback)(int16_t, int16_t, uint16_t, const char *)) {
  bool wrapped = false;
  u8g2.utf8_state = 0;
  const char *chrStart;
  while (true) {
    const uint16_t e = u8g2.utf8_next(*str++);
    if (e == 0x0fffe) continue;
    else if (e == 0x0ffff) break;
    else {
      const char *thisChrStart = chrStart;
      chrStart = str;
      if (e == '\n') {
        if (wrapped) continue;
        u8g2.println();
      } else if (e == '\r') u8g2.tx = 0;
      else {
        if (e == ' ' && wrapped) continue;
        if (callback) {
          const uint8_t cont = callback(u8g2.tx, u8g2.ty, e, thisChrStart);
          if (cont == 1) continue;
          else if (cont == 2) break;
        }
        u8g2.tx += u8g2.drawGlyph(u8g2.tx, u8g2.ty, e);
        wrapped = false;
        if (u8g2.tx + u8g2.u8g2.font_info.max_char_width > screen.width()) {
          u8g2.println();
          wrapped = true;
        }
      }
    }
  }
}

int select(const char **list, size_t count, size_t initial, bool bar) {
  int idx = initial, scroll = -u8g2.getFontAscent();
  while (true) {
    delay(20);
    updateInput();
    if (input.active()) {
      if (input.a) return -1;
      if (input.d) return idx;
      idx += input.c - input.b;
      idx = (idx + count) % count;
    }

    screen.fillScreen(0x0000);
    u8g2.setCursor(0, -(int16_t)scroll + BAR_HEIGHT * bar);
    for (size_t i = 0; i < count; i++) {
      if (i == idx) {
        int16_t top = u8g2.getCursorY() - u8g2.getFontAscent();
        int16_t bottom = u8g2.getCursorY() - u8g2.getFontDescent();
        if (top < 0) scroll += top;
        else if (bottom > screen.height()) scroll += bottom - screen.height();
      }
      printf("%s%s\n", i == idx ? "> " : "  ", list[i]);
    }
    updateDisplay();
  }
  return idx;
}
}