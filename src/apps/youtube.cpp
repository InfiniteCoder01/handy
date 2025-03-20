#include "youtube.h"
#include "hardware/bluetooth.h"
#include "hardware/display.h"
#include "hardware/input.h"
#include "hardware/wifi.h"
#include "utils.h"

#include <ebml.h>
#include <libyoutube.h>

namespace youtube {
volatile bool paused = false;
void avrcpCB(void *param, avrcp_operation_id_t op, int pressed) {
  (void)param;
  if (pressed && op == AVRCP_OPERATION_ID_PLAY) {
    paused = !paused;
  }
}

void show() {
  paused = false;
  bluetooth::a2dp.onAVRCP(avrcpCB);

  libyoutube::getStream("dQw4w9WgXcQ", 249);

  bool open = true;
  while (open) {
    input.update();
    wifi::tick();
    if (input.right.click())
      open = false;

    ui::screen.fillScreen(BLACK);
    ui::show();
  }
}
} // namespace youtube