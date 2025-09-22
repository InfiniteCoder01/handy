#include "hardware.hpp"

Input input;

void setupInput() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
}

void updateInput(bool allowSleep) {
  bool a = !digitalRead(5);
  bool b = !digitalRead(4);
  bool c = !digitalRead(3);
  bool d = !digitalRead(2);

  static uint32_t lastActive = 0;
  uint32_t t = millis();
  if (a != input.a || b != input.b || c != input.c || d != input.d || lastActive == 0) {
    lastActive = t;
    input.a = a;
    input.b = b;
    input.c = c;
    input.d = d;
  }
  input.lastActive = t - lastActive;
  if (allowSleep && input.lastActive > 10000 && !input.a && !input.b && !input.c && !input.d) {
    sleep();
    lastActive = millis();
  }
}