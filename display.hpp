#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "format.hpp"

extern uint8_t brightness;
extern GFXcanvas16 screen;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;

void setupDisplay();
void updateDisplay();
void displayPower(bool enabled);

uint16_t *loadImage(const char *filename);

namespace ui {
constexpr uint8_t BAR_HEIGHT = 16;
void bar();

void darken(uint8_t x, uint8_t y);

void defaultFont();
void print(const char *str, uint8_t (*callback)(int16_t, int16_t, uint16_t, const char *) = nullptr);

inline void println(const char *str) {
  print(str);
  print("\n");
}

template<typename... Args>
inline void printf(const char *fmt, Args... args) {
  print(format(fmt, args...).c_str());
}

int select(const char **list, size_t count, size_t initial = 0, bool bar = false);
}