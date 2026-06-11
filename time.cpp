#include "hardware.hpp"
#include "format.hpp"
#include "hardware/rtc.h"

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

static WiFiUDP ntpUDP;
static NTPClient ntp(ntpUDP);
datetime_t now;

void initRTC() {
  rtc_init();

  ntp.begin();
  ntp.setTimeOffset(3 * 60 * 60);
}

void updateRTC() {
  if (WiFi.isConnected() && ntp.update()) {
    time_t epoch = ntp.getEpochTime();
    tm tv;
    localtime_r(&epoch, &tv);
    now = {
      .year = (int16_t)(tv.tm_year + 1900),
      .month = (int8_t)(tv.tm_mon + 1),
      .day = (int8_t)tv.tm_mday,
      .dotw = (int8_t)tv.tm_wday,
      .hour = (int8_t)tv.tm_hour,
      .min = (int8_t)tv.tm_min,
      .sec = (int8_t)tv.tm_sec,
    };
    rtc_set_datetime(&now);
    sleep_us(64);
  } else rtc_get_datetime(&now);
}

String formatTime1() {
  return format("%02u:%02u", (now.hour + 11) % 12 + 1, now.min);
}

String formatTime2() {
  return format("%02u %s", now.sec, now.hour >= 12 ? "PM" : "AM");
}

String formatTime() {
  return formatTime1() + (now.hour >= 12 ? "PM" : "AM");
}

String formatDate() {
  const char *dow[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  const char *mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  return format("%s, %s %d, %d", dow[now.dotw], mon[now.month - 1],
                now.day, now.year);
}