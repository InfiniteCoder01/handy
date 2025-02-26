#include "settings.h"
#include "input.h"
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
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();
  }
}

const char *encToString(uint8_t enc) {
  switch (enc) {
  case ENC_TYPE_NONE:
    return "NONE";
  case ENC_TYPE_TKIP:
    return "WPA";
  case ENC_TYPE_CCMP:
    return "WPA2";
  case ENC_TYPE_AUTO:
    return "AUTO";
  }
  return "UNKN";
}

void wifi() {
  bool open = true;

  using namespace ui::shortcuts;
  ui::Container menu(true);
  menu << status::bar;
  menu << inl(button(label("Refresh"), []() { WiFi.scanNetworks(true); }),
              button(label("Exit"), [&open]() { open = false; }));

  const auto network = [](uint8_t idx) {
    // encToString(WiFi.encryptionType(idx))
    return button(
        inl(image(ICON_SIZE, icon(Icon::WiFi)), label(WiFi.SSID(idx))), []() {
          // WiFi.con
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
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();
  }
}
} // namespace settings