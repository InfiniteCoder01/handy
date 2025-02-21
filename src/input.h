#pragma once
#include "OreonMath.hpp"
#include <EncButton.h>

using namespace VectorMath;

extern struct Input {
  Button left = Button(6);
  Button right = Button(2);
  Button up = Button(4);
  Button down = Button(5);
  Button ok = Button(3);
  vec2i joy;
  bool active;
  uint32_t lastActive;

  bool charging;
  float voltage, percentage;

  void init();
  void update();
} input;