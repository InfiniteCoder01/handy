#pragma once
#include "OreonMath.hpp"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

using namespace VectorMath;

const uint16_t BLACK = 0x0000;
const uint16_t BLUE = 0x001F;
const uint16_t RED = 0xF800;
const uint16_t GREEN = 0x07E0;
const uint16_t CYAN = 0x07FF;
const uint16_t MAGENTA = 0xF81F;
const uint16_t YELLOW = 0xFFE0;
const uint16_t WHITE = 0xFFFF;

namespace ui {
extern GFXcanvas16 screen;
extern Adafruit_ST7735 tft;
extern uint8_t brightness;

void initializeDisplay();
void displayPower(bool enabled);
void show();

void drawImage(vec2i pos, vec2u size, const uint16_t *image);
} // namespace ui
