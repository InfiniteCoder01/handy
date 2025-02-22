#pragma once
#include "OreonMath.hpp"

const VectorMath::vec2u ICON_SIZE = VectorMath::vec2u(7, 7);
enum class Icon { WiFi, NoWiFi };
const uint16_t *icon(const Icon icon);