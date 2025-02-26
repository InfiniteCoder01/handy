#include "menu.h"
#include "app_icons.h"
#include "input.h"
#include "settings.h"
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
      button(image(APP_ICON_SIZE, appIcon(AppIcon::Exit)),
             [&open]() { open = false; }),
  });

  while (open) {
    input.update();
    ui::screen.fillScreen(BLACK);
    ui::serve(menu);
    ui::show();

    if (millis() - input.lastActive > 5000) {
      open = false;
    }
  }
}
} // namespace menu