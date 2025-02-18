#include "menu.h"
#include "input.h"
#include "ui/icons.h"
#include "ui/status.h"
#include "ui/ui.h"

namespace menu {
static ui::Container menu;
static bool open;

void createUI() {
  auto apps = std::make_shared<ui::Container>();
  *apps << new ui::Image(ui::ICON_SIZE, ui::icon(ui::Icon::Settings));
  *apps << new ui::Image(ui::ICON_SIZE, ui::icon(ui::Icon::Exit));

  menu << status::bar;
  menu << std::move(apps);
}

void show() {
  open = true;
  while (open) {
    input.update();
    ui::screen.fillScreen(BLACK);

    menu.layout(ui::screenSize());
    menu.draw(0);
    ui::show();

    if (millis() - input.lastActive > 5000) {
      open = false;
    }
  }
}
} // namespace menu