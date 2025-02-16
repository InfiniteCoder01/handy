#pragma once
#include <EncButton.h>

extern struct Input {
  Button left = Button(6);
  Button right = Button(2);
  Button up = Button(4);
  Button down = Button(5);
  Button ok = Button(3);
  uint32_t lastActive;

  bool charging;
  float voltage, percentage;

  void init();
  void update();
} input;