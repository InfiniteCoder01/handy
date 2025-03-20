#include "bluetooth.h"
#include <FreeRTOS.h>
#include <task.h>

namespace bluetooth {
A2DPSource a2dp;

static const int16_t SILENCE[64 * 2] = {0};
void silence() {
  if (!a2dp.connected())
    return;
  while (a2dp.availableForWrite() > 64)
    a2dp.write((uint8_t *)SILENCE, 64);
}
} // namespace bluetooth