#pragma once
#include <Arduino.h>

namespace power {
void init();
void sleep();
bool charging();
float voltage();
}