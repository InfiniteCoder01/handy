#include "menu.h"
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
      button(icon(ui::Icon::Settings), settings::show),
      button(icon(ui::Icon::Exit), [&open]() { open = false; }),
  });

  while (open) {
    input.update();
    ui::screen.fillScreen(BLACK);

    menu.layout(ui::screenSize());
    menu.draw(0, true);
    ui::show();

    if (millis() - input.lastActive > 5000) {
      open = false;
    }
  }
}
} // namespace menu