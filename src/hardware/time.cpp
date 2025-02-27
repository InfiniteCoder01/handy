#include "time.h"
#include "ui/ui.h"
#include "utils.h"

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

void updateRTC() { now = rtc.now(); }

String formatTime1() {
  return format("%02u:%02u", (now.hour() + 11) % 12 + 1, now.minute());
}

String formatTime2() { return now.hour() >= 12 ? "PM" : "AM"; }
String formatTime() { return formatTime1() + formatTime2(); }
String formatDate() {
  const char *dow[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char *mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  return format("%s, %s %d, %d", dow[now.dayOfWeek()], mon[now.month() - 1],
                now.day(), now.year());
}