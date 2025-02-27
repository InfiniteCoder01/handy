#pragma once
#include <Arduino.h>
#include <RTClib.h>

extern DateTime now;

void initRTC();
void updateRTC();
String formatTime1();
String formatTime2();
String formatTime();
String formatDate();