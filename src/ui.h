#pragma once
#include "display.h"
#include <vector>
#include <memory>

namespace ui {
inline vec2u screenSize() {
  return vec2u(screen.width(), screen.height());
}
void showSplash(const String &text, const uint16_t color = WHITE);

// ************************************************************** Nodes
struct Node {
  vec2i pos;
  vec2u computedSize;

  virtual ~Node() {}

  // Layout all the children relative to this node, compute the size, limited to available space
  virtual void layout(vec2u available) {
    computedSize = available;
  }

  // Draw relative to the offset (top-left of this item on the screen)
  virtual void draw(vec2i offset) {}
};

struct Container : Node {
  std::vector<std::unique_ptr<Node>> children;
  // Layout settings
  bool vertical = false, wrap = true;
  // Size override
  vec2u size = 0;

  enum Alignment {
    Start, Center, End, Fill
  } justifyContent = Alignment::Start;
  float alignItems = 0.0;

  virtual void layout(vec2u available) override;
  virtual void draw(vec2i offset) override;
};

struct Label : Node {
  String text;
  uint8_t size;
  uint16_t color;

  Label(String text, uint8_t size = 1, uint16_t color = WHITE)
    : text(text), size(size), color(color) {
  }

  void setScreenSettings(vec2i offset) const;
  virtual void layout(vec2u available) override;
  virtual void draw(vec2i offset) override;
};

struct Image : Node {
  const uint16_t *image;

  Image(vec2u size, const uint16_t *image)
    : image(image) {
    computedSize = size;
  }

  virtual void layout(vec2u available) override {}
  virtual void draw(vec2i offset) override;
};
}
