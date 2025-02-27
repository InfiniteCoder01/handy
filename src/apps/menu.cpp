#include "menu.h"
#include "app_icons.h"
#include "hardware/input.h"
#include "hardware/wifi.h"
#include "settings.h"
#include "ui/status.h"
#include "ui/ui.h"
#include "ytmusic.h"

namespace menu {
void show() {
  bool open = true;

  using namespace ui::shortcuts;
  ui::Container menu;
  menu << status::bar;
  menu << list({
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Settings)), settings::show),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::YTMusic)), ytmusic::show),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Exit)),
             [&open]() { open = false; }),
  });

  while (open) {
    input.update();
    wifi::tick();
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();

    if (millis() - input.lastActive > 5000) {
      open = false;
    }
  }
}
} // namespace menu