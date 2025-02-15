#pragma once
#include <vector>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "utils.h"
#include "OreonMath.hpp"
#include <memory>

using namespace Math;
using namespace VectorMath;

const uint16_t BLACK = 0x0000;
const uint16_t BLUE = 0x001F;
const uint16_t RED = 0xF800;
const uint16_t GREEN = 0x07E0;
const uint16_t CYAN = 0x07FF;
const uint16_t MAGENTA = 0xF81F;
const uint16_t YELLOW = 0xFFE0;
const uint16_t WHITE = 0xFFFF;

namespace ui {
extern GFXcanvas16 screen;
extern Adafruit_ST7735 tft;
extern uint8_t brightness;

void initializeDisplay();
void displayPower(bool enabled);
void show();

// uint16_t statusBar();
void showSplash(const String &text, const uint16_t color = WHITE);

// ************************************************************** Layout
struct Frame {
  vec2i offset;
  vec2u size;

  Frame(vec2i offset, vec2u size)
    : offset(offset), size(size) {}

  static Frame fullscreen() {
    return Frame(0, vec2u(screen.width(), screen.height()));
  }
};

struct Node {
  // Note: Set by the parent element before draw
  vec2i pos;
  vec2u size;

  virtual ~Node() {}

  virtual bool roughFit(const Frame &frame) {
    return size.x <= frame.size.x && size.y <= frame.size.y;
  }

  virtual void layout(const Frame &frame) {
    pos = frame.offset;
  }

  virtual void draw(bool focused) = 0;
};

struct Container : Node {
  std::vector<std::unique_ptr<Node>> children;
  bool vertical;
  // enum class Alignment {} justify_content, item_align;
  // vec2<int8_t> direction, alignment;

  virtual bool roughFit(const Frame &frame) override;
  virtual void layout(const Frame &frame) override;
  virtual void draw(bool focused) override;
};

struct Label : Node {
  String text;
  uint8_t size;
  uint16_t color;

  Label(String text, uint8_t size = 1, uint16_t color = WHITE)
    : text(text), size(size), color(color) {
    Node::size = 10;
  }

  virtual void draw(bool focused) override;
};
}
