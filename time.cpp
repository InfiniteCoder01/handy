#include "hardware.hpp"
#include "format.hpp"
#include "pico/aon_timer.h"

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

static WiFiUDP ntpUDP;
static NTPClient ntp(ntpUDP);
tm now;

void initRTC() {
  aon_timer_start_with_timeofday();

  ntp.begin();
  ntp.setTimeOffset(3 * 60 * 60);
}

void updateRTC() {
  if (WiFi.isConnected() && ntp.update()) {
    time_t epoch = ntp.getEpochTime();
    localtime_r(&epoch, &now);
    aon_timer_set_time_calendar(&now);
    sleep_us(64);
  } else aon_timer_get_time_calendar(&now);
}

String formatTime1() {
  return format("%02u:%02u", (now.tm_hour + 11) % 12 + 1, now.tm_min);
}

String formatTime2() {
  return format("%02u %s", now.tm_sec, now.tm_hour >= 12 ? "PM" : "AM");
}

String formatTime() {
  return formatTime1() + (now.tm_hour >= 12 ? "PM" : "AM");
}

String formatDate() {
  const char *dotw[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  const char *mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  return format("%s, %s %d, %d", dotw[now.tm_wday], mon[now.tm_mon],
                now.tm_mday, now.tm_year + 1900);
}