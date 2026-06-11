#include "hardware.hpp"

Input input;

void setupInput() {
  pinMode(28, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
}

void updateInput(bool allowSleep) {
  bool a = !digitalRead(28);
  bool b = !digitalRead(27);
  bool c = !digitalRead(26);
  bool d = !digitalRead(22);

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