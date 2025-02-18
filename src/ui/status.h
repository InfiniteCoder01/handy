#pragma once
#include "ui.h"

namespace status {
void createUI();

struct Battery : ui::Node {
  Battery() {}

  virtual void layout(vec2u available) override { computedSize = vec2u(4, 7); }
  virtual void draw(vec2i offset) override;
};

extern std::shared_ptr<ui::Container> bar;
} // namespace status