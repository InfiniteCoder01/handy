#include "ytmusic.h"
#include "hardware/display.h"
#include "hardware/input.h"
#include "raw.h"
#include <Arduino.h>
#include <BluetoothAudio.h>

namespace ytmusic {
A2DPSource a2dp;

int16_t pcm[64 * 2];
uint32_t phase = 0;
volatile bool paused = false;

void avrcpCB(void *param, avrcp_operation_id_t op, int pressed) {
  (void)param;
  if (pressed && op == AVRCP_OPERATION_ID_PLAY) {
    paused = !paused;
    if (paused) {
      Serial.printf("Pausing\n");
    } else {
      Serial.printf("Resuming\n");
    }
  }
}

void volumeCB(void *param, int pct) {
  (void)param;
  Serial.printf("Speaker volume changed to %d%%\n", pct);
}

void connectCB(void *param, bool connected) {
  (void)param;
  if (connected) {
    Serial.printf("A2DP connection started to %s\n",
                  bd_addr_to_str(a2dp.getSinkAddress()));
  } else {
    Serial.printf("A2DP connection stopped\n");
  }
}

void fillPCM() {
  if (paused) {
    bzero(pcm, sizeof(pcm));
    return;
  }
  for (int i = 0; i < 64; i++) {
    // Audio in flash is 8-bit signed mono, so shift left to make 16-bit and
    // then play over both channels
    pcm[i * 2] =
        auclairedelalune_raw[(i + phase) % sizeof(auclairedelalune_raw)] << 8;
    pcm[i * 2 + 1] =
        auclairedelalune_raw[(i + phase) % sizeof(auclairedelalune_raw)] << 8;
  }
  phase += 64;
}

void show() {
  a2dp.onAVRCP(avrcpCB);
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.begin();
  ui::screen.fillScreen(BLACK);
  ui::screen.setCursor(0, 0);
  while (!input.right.click()) {
    input.update();
    while ((size_t)a2dp.availableForWrite() > sizeof(pcm)) {
      fillPCM();
      a2dp.write((const uint8_t *)pcm, sizeof(pcm));
    }
    if (input.up.click()) {
      ui::screen.print("Connecting...");
      a2dp.disconnect();
      a2dp.clearPairing();
      if (a2dp.connect()) {
        ui::screen.println("Connected!");
      } else {
        ui::screen.println("Failed!  :(");
      }
    }
    ui::show();
  }
}
} // namespace ytmusic