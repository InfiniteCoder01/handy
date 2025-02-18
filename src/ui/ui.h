#pragma once
#include "display.h"
#include "functional"
#include "memory"
#include "optional"
#include "vector"

namespace ui {
struct Container;

inline vec2u screenSize() { return vec2u(screen.width(), screen.height()); }
void showSplash(const String &text, const uint16_t color = WHITE);
void serve(Container &container);

// ************************************************************** Nodes
struct Node {
  vec2i pos;
  vec2u computedSize;

  virtual ~Node() = default;

  virtual bool focusable() { return false; }

  // Layout all the children relative to this node, compute the size,
  // limited to available space
  virtual void layout(vec2u available) { computedSize = available; }

  // Draw relative to the offset (top-left of this item on the screen)
  virtual void draw(vec2i offset) {}
};

struct Container : Node {
  std::vector<std::shared_ptr<Node>> children;
  // Layout settings
  bool vertical = false, wrap = true;
  // Size override
  vec2u size = 0;
  // Spacing
  uint16_t spacing = 1;

  enum Alignment { Start, Center, End, Fill } justifyContent = Alignment::Start;
  float alignItems = 0.0;

  Container(bool vertical = false, Alignment justifyContent = Alignment::Start,
            float alignItems = 0.0)
      : vertical(vertical), justifyContent(justifyContent),
        alignItems(alignItems) {}

  virtual ~Container() = default;

  Container &operator<<(Node *node) {
    children.push_back(std::shared_ptr<Node>(node));
    return *this;
  }

  Container &operator<<(std::shared_ptr<Node> &&node) {
    children.push_back(std::move(node));
    return *this;
  }

  virtual void layout(vec2u available) override;
  virtual void draw(vec2i offset) override;
};

struct Label : Node {
  String text = "";
  uint8_t size = 1;
  uint16_t color = WHITE;
  const GFXfont *font = nullptr;

  Label(String text, uint8_t size = 1, uint16_t color = WHITE,
        const GFXfont *font = nullptr)
      : text(text), size(size), color(color), font(font) {}

  virtual ~Label() = default;

  void setScreenSettings(vec2i offset) const;
  virtual void layout(vec2u available) override;
  virtual void draw(vec2i offset) override;
};

struct FunctionalLabel : Label {
  std::function<String()> source;

  FunctionalLabel(std::function<String()> source, uint8_t size = 1,
                  uint16_t color = WHITE, const GFXfont *font = nullptr)
      : source(source), Label(source(), size, color, font) {}

  virtual ~FunctionalLabel() = default;
  virtual void layout(vec2u available) override;
};

struct Image : Node {
  const uint16_t *image;
  std::optional<std::function<void()>> onClick;

  Image(vec2u size, const uint16_t *image,
        std::optional<std::function<void()>> onClick = std::nullopt)
      : image(image), onClick(onClick) {
    computedSize = size;
  }

  virtual bool focusable() override { return onClick.has_value(); }
  virtual void layout(vec2u available) override {}
  virtual void draw(vec2i offset) override;
};
} // namespace ui
