#include "ui.h"
#include "hardware/display.h"
#include "hardware/input.h"
#include "status.h"

namespace ui {
void showSplash(const String &text, const uint16_t color) {
  screen.fillScreen(BLACK);
  screen.setTextColor(color);
  screen.setTextSize(2);
  int16_t _;
  uint16_t w, h;
  screen.getTextBounds(text, 0, 0, &_, &_, &w, &h);
  screen.setCursor((screen.width() - w) / 2, (screen.height() - h) / 2);
  screen.print(text);
  show();
}

String prompt(std::shared_ptr<Node> prompt) {
  const String keymap = "\0171234567890-=\010\nabcdefghijklmnopqrstuvwxyz";
  const String keymapShift = "\017!?.,%?&*()_+\010\nABCDEFGHIJKLMNOPQRSTUVWXYZ";

  bool shift = false, done = false;
  String text = "";

  using namespace ui::shortcuts;
  auto keyboard = std::make_shared<Container>();
  for (size_t i = 0; i < keymap.length(); i++) {
    const auto ch = [i, &shift, &keymap, &keymapShift]() {
      return (shift ? keymapShift : keymap)[i];
    };
    *keyboard << button(flabel([ch]() {
                          char key = ch();
                          if (key == '\017')
                            return String("Sh");
                          if (key == '\010')
                            return String("<-");
                          if (key == '\n')
                            return String("En");
                          else
                            return String(key);
                        }),
                        [ch, &shift, &done, &text]() {
                          char key = ch();
                          if (key == '\017')
                            shift = !shift;
                          else if (key == '\010')
                            text.remove(text.length() - 1);
                          else if (key == '\n')
                            done = true;
                          else {
                            text += key;
                            shift = false;
                          }
                        });
  }

  keyboard->layout(screenSize());
  auto promptEl =
      center({prompt}, screenSize() - vec2u(0, keyboard->computedSize.y + 20));

  ui::Container layout(true);
  layout << status::bar;
  layout << std::move(promptEl);
  layout << flabel([&text]() { return text; });
  layout << std::move(keyboard);

  while (!done) {
    if (Serial.available()) {
      text = Serial.readStringUntil('\n');
      done = true;
    }

    input.update();
    ui::screen.fillScreen(BLACK);
    ui::serve(layout);
    ui::show();
  }
  return text;
}

void serve(Node &root) {
  root.layout(ui::screenSize());
  if (input.active && input.joy.dot(1)) {
    if (!root.focusNext())
      root.focus();
  }
  root.draw(0, true);
}

// ************************************************************** Container
bool Container::focusable() {
  for (auto &child : children) {
    if (child->focusable())
      return true;
  }
  return false;
}

void Container::layout(vec2u available) {
  computedSize = 0;
  vec2i v = 0;
  vec2i mainDir = vertical ? vec2i(0, 1) : vec2i(1, 0);
  vec2i offDir = vertical ? vec2i(1, 0) : vec2i(0, 1);
  uint16_t rowWidth = 0;
  std::vector<size_t> splits;
  for (size_t i = 0; i < children.size(); i++) {
    children[i]->pos = v;
    children[i]->layout(available - v);
    if (wrap &&
        (v + children[i]->computedSize).dot(mainDir) > available.dot(mainDir)) {
      // New row
      splits.push_back(i);
      v *= offDir;
      v += offDir * (rowWidth + spacing);
      rowWidth = 0;
      children[i]->pos = v;
      children[i]->layout(available - v);
    }
    computedSize =
        VectorMath::max(computedSize, (vec2u)(v + children[i]->computedSize));

    rowWidth =
        Math::max(rowWidth, (uint16_t)children[i]->computedSize.dot(offDir));
    v += mainDir * children[i]->computedSize + mainDir * spacing;
  }
  splits.push_back(children.size());

  vec2u packedSize = computedSize;
  if (size.x > 0)
    computedSize.x = size.x;
  if (size.y > 0)
    computedSize.y = size.y;

  size_t rowStart = 0;
  for (const size_t rowEnd : splits) {
    rowWidth = 0;
    uint16_t rowLength = 0;
    for (size_t i = rowStart; i < rowEnd; i++) {
      rowWidth =
          Math::max(rowWidth, (uint16_t)children[i]->computedSize.dot(offDir));
      rowLength = (children[i]->pos + children[i]->computedSize).dot(mainDir);
    }

    float offset = (computedSize.dot(mainDir) - rowLength) * justifyContent;
    for (size_t i = rowStart; i < rowEnd; i++) {
      children[i]->pos +=
          (offDir * rowWidth - children[i]->computedSize * offDir) *
              alignItems +
          mainDir * offset;
    }
    rowStart = rowEnd;
  }
}

void Container::focus() {
  int8_t dir = input.joy.dot(1);
  cursor = dir > 0 ? 0 : children.size() - 1;
  while (cursor < children.size()) {
    if (children[cursor]->focusable()) {
      children[cursor]->focus();
      break;
    }
    cursor += dir;
  }
}

bool Container::focusNext() {
  if (cursor >= children.size())
    return false;
  if (children[cursor]->focusNext())
    return true;

  int8_t dir = input.joy.dot(1);
  while (true) {
    cursor += dir;
    if (cursor >= children.size())
      return false;
    if (children[cursor]->focusable()) {
      children[cursor]->focus();
      return true;
    }
    cursor += dir;
  }
  return false;
}

void Container::draw(vec2i offset, bool focused) {
  while (cursor < children.size() && !children[cursor]->focusable())
    cursor++;
  for (size_t i = 0; i < children.size(); i++) {
    children[i]->draw(offset + children[i]->pos, focused && cursor == i);
  }
}

// ************************************************************** Button
void Button::layout(vec2u available) {
  kid->pos = pos + padding;
  kid->layout(available - padding * 2);
  computedSize = kid->computedSize + padding * 2;
}

void Button::draw(vec2i offset, bool focused) {
  kid->draw(offset + padding, false);
  if (focused) {
    screen.drawRect(offset.x, offset.y, computedSize.x, computedSize.y, RED);
    if (input.ok.click())
      onClick();
  }
}

// ************************************************************** Label
void Label::setScreenSettings(vec2i offset) const {
  screen.setFont(font);
  screen.setCursor(offset.x, offset.y + (font ? 10 * size : 0));
  screen.setTextSize(size);
  screen.setTextColor(color);
  screen.setTextWrap(false);
}

void Label::layout(vec2u available) {
  setScreenSettings(0);
  int16_t x1, y1;
  uint16_t w, h;
  screen.getTextBounds(text, screen.getCursorX(), screen.getCursorY(), &x1, &y1,
                       &w, &h);
  computedSize = vec2u(w, h) + focusable() * 2;
}

void Label::draw(vec2i offset, bool focused) {
  setScreenSettings(offset + focusable());
  screen.print(text);
}

// *********************************************************** FunctionalLabel
void FunctionalLabel::layout(vec2u available) {
  text = source();
  Label::layout(available);
}

// ************************************************************** Image
void Image::draw(vec2i offset, bool focused) {
  drawImage(offset, computedSize, image);
}

// *********************************************************** FunctionalImage
void FunctionalImage::layout(vec2u available) {
  image = source();
  Image::layout(available);
}
} // namespace ui