#include "ui.h"
#include "display.h"
#include "input.h"

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

  float justifyContentAlignment = 0.0;
  if (justifyContent == Alignment::Center)
    justifyContentAlignment = 0.5;
  else if (justifyContent == Alignment::End)
    justifyContentAlignment = 1.0;

  size_t rowStart = 0;
  for (const size_t rowEnd : splits) {
    rowWidth = 0;
    uint16_t rowLength = 0;
    for (size_t i = rowStart; i < rowEnd; i++) {
      rowWidth =
          Math::max(rowWidth, (uint16_t)children[i]->computedSize.dot(offDir));
      rowLength = (children[i]->pos + children[i]->computedSize).dot(mainDir);
    }

    float offset =
        (computedSize.dot(mainDir) - rowLength) * justifyContentAlignment;
    float spacing = justifyContent == Alignment::Fill
                        ? (computedSize.dot(mainDir) - rowLength) /
                              (float)Math::max(rowEnd - rowStart - 1, 1)
                        : 0.0;
    for (size_t i = rowStart; i < rowEnd; i++) {
      children[i]->pos +=
          (offDir * rowWidth - children[i]->computedSize * offDir) *
              alignItems +
          mainDir * offset;
      offset += spacing;
    }
    rowStart = rowEnd;
  }
}

bool Container::draw(vec2i offset, bool focused) {
  if (focused) {
    while (cursor < children.size() && !children[cursor]->focusable())
      cursor++;
    if (cursor >= children.size())
      cursor = 0;
  }

  bool selectedHeld = false;
  for (size_t i = 0; i < children.size(); i++) {
    bool selected = focused && cursor == i;
    bool hold = children[i]->draw(offset + children[i]->pos, selected);
    if (selected)
      selectedHeld = hold;
  }

  if (selectedHeld)
    return true;

  if (input.active) {
    bool hold = false;
    while (input.joy.x > 0 ? cursor + 1 < children.size() : cursor > 0) {
      cursor += input.joy.x;
      if (children[cursor]->focusable()) {
        hold = true;
        break;
      }
    }
    if (!hold)
      cursor = 0;

    return hold;
  }

  return false;
}

// ************************************************************** Button
void Button::layout(vec2u available) {
  kid->pos = pos + padding;
  kid->layout(available - padding * 2);
  computedSize = kid->computedSize + padding * 2;
}

bool Button::draw(vec2i offset, bool focused) {
  bool hold = kid->draw(offset + padding, focused);
  if (focused) {
    screen.drawRect(offset.x, offset.y, computedSize.x, computedSize.y, RED);
    if (input.ok.click())
      onClick();
  }
  return hold;
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

bool Label::draw(vec2i offset, bool focused) {
  setScreenSettings(offset + focusable());
  screen.print(text);
  return false;
}

// *********************************************************** FunctionalLabel
void FunctionalLabel::layout(vec2u available) {
  text = source();
  Label::layout(available);
}

// ************************************************************** Image
bool Image::draw(vec2i offset, bool focused) {
  drawImage(offset, computedSize, image);
  return false;
}

// *********************************************************** FunctionalImage
void FunctionalImage::layout(vec2u available) {
  image = source();
  Image::layout(available);
}
} // namespace ui