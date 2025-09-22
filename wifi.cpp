#include "hardware.hpp"
#include <WiFi.h>
#include <algorithm>

std::map<String, String> knownWiFiNetworks;

void updateWiFi() {
  static bool lastScanned = false;
  if (WiFi.status() == WL_CONNECTED || WiFi.status() == WL_DISCONNECTED) {
    lastScanned = false;
    return;
  }

  if (!lastScanned) {
    WiFi.scanNetworks(true);
    lastScanned = true;
  }

  auto cnt = WiFi.scanComplete();
  if (cnt >= 0) {
    std::vector<uint8_t> networks;
    networks.resize(cnt);
    for (uint32_t i = 0; i < cnt; i++) {
      networks[i] = i;
    }

    std::sort(networks.begin(), networks.end(),
              [](const uint8_t a, const uint8_t b) {
                return WiFi.RSSI(a) > WiFi.RSSI(b);
              });
    for (const auto net : networks) {
      const auto psk = knownWiFiNetworks.find(WiFi.SSID(net));
      if (psk == knownWiFiNetworks.end())
        continue;

      uint8_t bssid[6];
      WiFi.beginNoBlock(WiFi.SSID(net),
                        psk->second.length() == 0 ? nullptr
                                                  : psk->second.c_str(),
                        WiFi.BSSID(net, bssid));
    }

    lastScanned = false;
  }
}