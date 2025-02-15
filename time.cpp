#include "time.h"
#include "ui.h"

DateTime now = DateTime(__DATE__, __TIME__);
static DS1302 rtc(9, 7, 8);

void initRTC() {
  rtc.begin();

  if (!rtc.isrunning() || now > rtc.now()) {
    ui::showSplash("Reset RTC!", RED);
    rtc.adjust(now);
    delay(1000);
  }
  now = rtc.now();
}

void updateRTC() {
  now = rtc.now();
}