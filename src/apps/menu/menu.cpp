#include "menu.h"
#include "apps/app_icons.h"
#include "apps/flap/flap.h"
#include "apps/settings/settings.h"
#include "apps/youtube/youtube.h"
#include "apps/stopwatch/stopwatch.h"
#include "hardware/bluetooth.h"
#include "hardware/input.h"
#include "hardware/wifi.h"
#include "ui/status.h"
#include "ui/ui.h"

namespace menu {
void show() {
  bool open = true;

  using namespace ui::shortcuts;
  ui::Container menu;
  menu << status::bar;
  menu << list({
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Settings)), settings::show),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Stopwatch)), stopwatch::show),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::YouTube)), youtube::show),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Flap)), flap::play),
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Exit)),
             [&open]() { open = false; }),
  });

  while (open) {
    input.update();
    wifi::tick();
    bluetooth::silence();
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();

    if (millis() - input.lastActive > 5000) {
      open = false;
    }
  }
}
} // namespace menu