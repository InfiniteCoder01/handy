#include "wifi.h"
#include <WiFi.h>
#include <algorithm>
#include <utility>
#include <vector>

namespace wifi {
std::map<String, String> knownNetworks;

bool lastScanned = false;
void tick(bool scan) {
  if (WiFi.status() == WL_CONNECTED || WiFi.status() == WL_DISCONNECTED) {
    lastScanned = false;
    return;
  }

  if (!lastScanned && scan) {
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
      const auto psk = knownNetworks.find(WiFi.SSID(net));
      if (psk == knownNetworks.end())
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
} // namespace wifi