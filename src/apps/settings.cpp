#include "settings.h"
#include "hardware/input.h"
#include "hardware/power.h"
#include "hardware/wifi.h"
#include "ui/icons.h"
#include "ui/status.h"
#include "ui/ui.h"
#include "utils.h"

#include <WiFi.h>

namespace settings {
void show() {
  bool open = true;

  using namespace ui::shortcuts;
  ui::Container menu(true);
  menu << status::bar;
  menu << flabel([]() {
    return input.charging ? "Battery is charging"
                          : format("Battery Voltage: %.2f", input.voltage);
  });
  menu << button(inl(image(ICON_SIZE, icon(Icon::WiFi)), label("WiFi")), wifi);
  menu << button(label("Exit"), [&open]() { open = false; });

  while (open) {
    input.update();
    wifi::tick();
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();
  }
}

void wifi() {
  bool open = true;

  using namespace ui::shortcuts;
  ui::Container menu(true);
  menu << status::bar;
  menu << inl(button(label("Refresh"), []() { WiFi.scanNetworks(true); }),
              button(label("Exit"), [&open]() { open = false; }));

  const auto network = [](uint8_t idx) {
    String ssid = WiFi.SSID(idx);
    uint8_t enc = WiFi.encryptionType(idx);
    const char *encs = "UNKN";
    if (enc == ENC_TYPE_NONE)
      encs = "NONE";
    else if (enc == ENC_TYPE_TKIP)
      encs = "WPA";
    else if (enc == ENC_TYPE_CCMP)
      encs = "WPA2";
    else if (enc == ENC_TYPE_AUTO)
      encs = "AUTO";

    return button(inl(image(ICON_SIZE, icon(Icon::WiFi)),
                      label(format("%s %s", ssid.c_str(), encs))),
                  [ssid, enc]() {
                    String password;
                    if (enc != ENC_TYPE_NONE)
                      password = ui::prompt(
                          vcenter({label("Enter password for"), label(ssid)}));

                    wifi::knownNetworks[ssid] = password;
                  });
  };
  auto networks = std::make_shared<ui::Container>(true);
  menu << networks;

  WiFi.scanNetworks(true);
  while (open) {
    auto cnt = WiFi.scanComplete();
    if (cnt >= 0) {
      networks->clear();
      for (auto i = 0; i < cnt; i++)
        *networks << network(i);
    }

    input.update();
    wifi::tick(false);
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();
  }
}
} // namespace settings