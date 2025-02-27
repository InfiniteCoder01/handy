#pragma once
#include "OreonMath.hpp"
#include <Arduino.h>

const VectorMath::vec2u APP_ICON_SIZE = VectorMath::vec2u(20, 20);
enum class AppIcon { Exit, Settings, YTMusic };
const uint16_t *appIcon(const AppIcon app);