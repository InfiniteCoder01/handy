#pragma once
#include "display.h"
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace ui {
struct Container;

inline vec2u screenSize() { return vec2u(screen.width(), screen.height()); }
void showSplash(const String &text, const uint16_t color = WHITE);

// ************************************************************** Nodes
struct Node {
  vec2i pos = 0;
  vec2u computedSize = 0;

  virtual ~Node() = default;

  virtual bool focusable() { return false; }

  // Layout all the children relative to this node, compute the size,
  // limited to available space
  virtual void layout(vec2u available) { computedSize = available; }

  // Draw relative to the offset (top-left of this item on the screen), returns
  // true if holds focus
  virtual bool draw(vec2i offset, bool focused) { return false; }
};

struct Container : Node {
  std::vector<std::shared_ptr<Node>> children;
  // Layout settings
  bool vertical = false, wrap = true;
  // Size override
  vec2u size = 0;
  // Spacing
  uint16_t spacing = 1;

  uint16_t cursor = 0;

  float alignItems = 0.0, justifyContent = 0.0;

  Container(bool vertical = false) : Node(), vertical(vertical) {}

  virtual ~Container() = default;

  Container &operator<<(Node *node) {
    children.push_back(std::shared_ptr<Node>(node));
    return *this;
  }

  Container &operator<<(std::shared_ptr<Node> &&node) {
    children.push_back(std::move(node));
    return *this;
  }

  virtual bool focusable() override;
  virtual void layout(vec2u available) override;
  virtual bool draw(vec2i offset, bool focused) override;
};

struct Button : Node {
  std::shared_ptr<Node> kid;
  std::function<void()> onClick;
  uint8_t padding = 1;

  Button(std::shared_ptr<Node> kid, std::function<void()> onClick)
      : Node(), kid(kid), onClick(onClick) {}

  Button(Node *kid, std::function<void()> onClick)
      : Button(std::shared_ptr<Node>(kid), onClick) {}

  virtual bool focusable() override { return true; }
  virtual void layout(vec2u available) override;
  virtual bool draw(vec2i offset, bool focused) override;
};

struct Label : Node {
  String text = "";
  uint8_t size = 1;
  uint16_t color = WHITE;
  const GFXfont *font = nullptr;

  Label(String text, uint8_t size = 1, uint16_t color = WHITE,
        const GFXfont *font = nullptr)
      : Node(), text(text), size(size), color(color), font(font) {}

  virtual ~Label() = default;

  void setScreenSettings(vec2i offset) const;
  virtual void layout(vec2u available) override;
  virtual bool draw(vec2i offset, bool focused) override;
};

struct FunctionalLabel : Label {
  std::function<String()> source;

  FunctionalLabel(std::function<String()> source, uint8_t size = 1,
                  uint16_t color = WHITE, const GFXfont *font = nullptr)
      : Label(source(), size, color, font), source(source) {}

  virtual ~FunctionalLabel() = default;
  virtual void layout(vec2u available) override;
};

struct Image : Node {
  const uint16_t *image;

  Image(vec2u size, const uint16_t *image) : Node(), image(image) {
    computedSize = size;
  }

  virtual void layout(vec2u available) override {}
  virtual bool draw(vec2i offset, bool focused) override;
};

struct FunctionalImage : Image {
  std::function<const uint16_t *()> source;

  FunctionalImage(vec2u size, std::function<const uint16_t *()> source)
      : Image(size, source()), source(source) {}

  virtual void layout(vec2u available) override;
};

namespace shortcuts {
inline std::shared_ptr<Node>
list(std::initializer_list<std::shared_ptr<Node>> nodes) {
  std::shared_ptr<Container> container = std::make_shared<Container>();
  container->children = nodes;
  return container;
}

inline std::shared_ptr<Node>
vlist(std::initializer_list<std::shared_ptr<Node>> nodes) {
  std::shared_ptr<Container> container = std::make_shared<Container>(true);
  container->children = nodes;
  return container;
}

inline std::shared_ptr<Node>
center(std::initializer_list<std::shared_ptr<Node>> nodes, vec2u size = 0) {
  std::shared_ptr<Container> container = std::make_shared<Container>();
  container->justifyContent = 0.5;
  container->alignItems = 0.5;
  container->size = size;
  container->children = nodes;
  return container;
}

inline std::shared_ptr<Node>
vcenter(std::initializer_list<std::shared_ptr<Node>> nodes, vec2u size = 0) {
  std::shared_ptr<Container> container = std::make_shared<Container>(true);
  container->justifyContent = 0.5;
  container->alignItems = 0.5;
  container->size = size;
  container->children = nodes;
  return container;
}

inline std::shared_ptr<Node>
aside(std::initializer_list<std::shared_ptr<Node>> nodes) {
  std::shared_ptr<Container> container = std::make_shared<Container>(true);
  container->wrap = false;
  container->alignItems = 1.0;
  container->children = nodes;
  return container;
}

template <typename... Nodes> inline std::shared_ptr<Node> inl(Nodes... nodes) {
  std::shared_ptr<Container> container = std::make_shared<Container>();
  container->wrap = false;
  container->alignItems = 0.5;
  container->children = {nodes...};
  return container;
}

template <typename... Nodes>
inline std::shared_ptr<Node> baseline(Nodes... nodes) {
  std::shared_ptr<Container> container = std::make_shared<Container>();
  container->wrap = false;
  container->alignItems = 1;
  container->children = {nodes...};
  return container;
}

inline std::shared_ptr<Node> button(std::shared_ptr<Node> kid,
                                    std::function<void()> onClick) {
  return std::make_shared<Button>(kid, onClick);
}

inline std::shared_ptr<Node> label(String text, uint8_t size = 1,
                                   uint16_t color = WHITE,
                                   const GFXfont *font = nullptr) {
  return std::make_shared<Label>(text, size, color, font);
}

inline std::shared_ptr<Node> flabel(std::function<String()> source,
                                    uint8_t size = 1, uint16_t color = WHITE,
                                    const GFXfont *font = nullptr) {
  return std::make_shared<FunctionalLabel>(source, size, color, font);
}

inline std::shared_ptr<Node> image(vec2u size, const uint16_t *image) {
  return std::make_shared<Image>(size, image);
}

inline std::shared_ptr<Node> fimage(vec2u size,
                                    std::function<const uint16_t *()> source) {
  return std::make_shared<FunctionalImage>(size, source);
}
} // namespace shortcuts
} // namespace ui
