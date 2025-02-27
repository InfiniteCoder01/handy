#include "status.h"
#include "hardware/input.h"
#include "hardware/time.h"
#include "icons.h"
#include "utils.h"
#include <WiFi.h>

namespace status {
std::shared_ptr<ui::Container> bar = std::make_shared<ui::Container>();

static const uint16_t batteryIcons[] = {
    0xf81f, 0xffff, 0xffff, 0xf81f, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0x5aaa, 0x5aaa, 0xffff, 0xffff, 0x5aaa, 0x5aaa, 0xffff,
    0xffff, 0x5aaa, 0x5aaa, 0xffff, 0xffff, 0x5aaa, 0x5aaa, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xf81f, 0xffff, 0xffff, 0xf81f,
    0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff,
    0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff,
    0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};

void Battery::draw(vec2i offset, bool focused) {
  if (input.charging) {
    ui::drawImage(offset, computedSize,
                  batteryIcons + computedSize.x * computedSize.y);
  } else {
    ui::drawImage(offset, computedSize, batteryIcons);

    uint16_t color = MAGENTA;
    if (input.percentage > 0.7) {
      color = GREEN;
    } else if (input.percentage > 0.5) {
      color = YELLOW;
    } else {
      color = RED;
    }

    const uint16_t height =
        Math::round((computedSize.y - 3) * input.percentage);
    ui::screen.fillRect(offset.x + 1, offset.y + computedSize.y - height - 1,
                        computedSize.x - 2, height, color);
  }
}

void createUI() {
  using namespace ui::shortcuts;
  bar->size.x = ui::screen.width();
  bar->wrap = false;
  *bar << new Battery();
  *bar << fimage(ICON_SIZE, []() {
    return icon(WiFi.isConnected() ? Icon::WiFi : Icon::NoWiFi);
  });
  *bar << flabel(formatTime);
}
} // namespace status