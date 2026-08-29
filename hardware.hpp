#pragma once
#include <Arduino.h>
#include <time.h>
#include <map>

// Input
struct Input {
  bool a = false;
  bool b = false;
  bool c = false;
  bool d = false;
  uint32_t lastActive = 0;

  bool active() { return lastActive == 0 || lastActive > 1000; }
};

extern Input input;
void setupInput();
void updateInput(bool allowSleep = false);

// WiFi
extern std::map<String, String> knownWiFiNetworks;
void updateWiFi();

// Time
extern tm now;

void initRTC();
void updateRTC();
String formatTime1();
String formatTime2();
String formatTime();
String formatDate();

// Power
void initPowerManagement();
bool charging();
float voltage();
void sleep();