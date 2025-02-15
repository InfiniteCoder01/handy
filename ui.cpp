#include "ui.h"

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
  while (tft.dmaBusy())
    ;
  analogWrite(BACKLIGHT, brightness);
}

void showSplash(const String &text, const uint16_t color) {
  screen.fillScreen(BLACK);
  screen.setTextColor(color);
  screen.setTextSize(2);
  int16_t _;
  uint16_t w, h;
  screen.getTextBounds(text, 0, 0, &_, &_, &w, &h);
  screen.setCursor((screen.width() - w) / 2, (screen.height() - h) / 2);
  screen.print(text);
  show();
}

// ************************************************************** Container
bool Container::roughFit(const Frame &frame) {
  vec2i v = 0;
  for (auto &child : children) {
    // child->roughFit()
  }
  return true;
}

void Container::layout(const Frame &frame) {
  pos = frame.offset;
  vec2i p = pos;
  for (size_t i = 0; i < children.size(); i++) {
    children[i]->layout(Frame(p, frame.size + pos - p));
    if (vertical) p.y += children[i]->size.y;
    else p.x += children[i]->size.x;
  }
}

void Container::draw(bool focused) {
  for (size_t i = 0; i < children.size(); i++) {
    children[i]->draw(focused && i);
  }
}

// ************************************************************** Label
void Label::draw(bool focused) {
  screen.setCursor(pos.x, pos.y);
  screen.setTextSize(size);
  screen.setTextColor(color);
  screen.print(text);
}

// vec2u Label::minSize() {
//   int16_t _;
//   uint16_t w, h;
//   screen.setTextSize(size);
//   screen.getTextBounds(text, 0, 0, &_, &_, &w, &h);
//   return vec2u(w, h);
// }
}