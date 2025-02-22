#include "settings.h"
#include "input.h"
#include "ui/icons.h"
#include "ui/status.h"
#include "ui/ui.h"
#include "utils.h"

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
  menu << button(inl(image(ICON_SIZE, icon(Icon::WiFi)), label("WiFi")),
                 []() {});
  menu << button(label("Exit"), [&open]() { open = false; });

  while (open) {
    input.update();
    ui::screen.fillScreen(BLACK);

    menu.layout(ui::screenSize());
    menu.draw(0, true);
    ui::show();
  }
}
} // namespace settings