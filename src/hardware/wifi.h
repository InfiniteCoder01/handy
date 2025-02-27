#pragma once
#include <Arduino.h>
#include <map>

namespace wifi {
extern std::map<String, String> knownNetworks;

void tick(bool scan = true);
} // namespace wifi