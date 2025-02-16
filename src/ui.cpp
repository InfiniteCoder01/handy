#include "display.h"
#include "ui.h"

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
    if (wrap && (v + children[i]->computedSize).dot(mainDir) > available.dot(mainDir)) {
      // New row
      splits.push_back(i);
      v *= offDir;
      v += offDir * rowWidth;
      rowWidth = 0;
      children[i]->pos = v;
      children[i]->layout(available - v);
    }
    computedSize = VectorMath::max(computedSize, (vec2u)(v + children[i]->computedSize));

    rowWidth = Math::max(rowWidth, (uint16_t)children[i]->computedSize.dot(offDir));
    v += mainDir * children[i]->computedSize;
  }
  splits.push_back(children.size());

  vec2u packedSize = computedSize;
  if (size.x > 0) computedSize.x = size.x;
  if (size.y > 0) computedSize.y = size.y;

  float justifyContentAlignment = 0.0;
  if (justifyContent == Alignment::Center) justifyContentAlignment = 0.5;
  else if (justifyContent == Alignment::End) justifyContentAlignment = 1.0;

  size_t rowStart = 0;
  for (const size_t rowEnd : splits) {
    rowWidth = 0;
    uint16_t rowLength = 0;
    for (size_t i = rowStart; i < rowEnd; i++) {
      rowWidth = Math::max(rowWidth, (uint16_t)children[i]->computedSize.dot(offDir));
      rowLength = (children[i]->pos + children[i]->computedSize).dot(mainDir);
    }

    float offset = (computedSize.dot(mainDir) - rowLength) * justifyContentAlignment;
    float spacing = justifyContent == Alignment::Fill ? (computedSize.dot(mainDir) - rowLength) / (float)Math::max(rowEnd - rowStart - 1, 1) : 0.0;
    for (size_t i = rowStart; i < rowEnd; i++) {
      children[i]->pos += (offDir * rowWidth - children[i]->computedSize * offDir) * alignItems + mainDir * offset;
      offset += spacing;
    }
    rowStart = rowEnd;
  }
}

void Container::draw(vec2i offset) {
  for (auto &child : children) {
    child->draw(offset + child->pos);
  }
}

// ************************************************************** Label
void Label::setScreenSettings(vec2i offset) const {
  screen.setCursor(offset.x, offset.y);
  screen.setTextSize(size);
  screen.setTextColor(color);
  screen.setTextWrap(false);
}

void Label::layout(vec2u available) {
  setScreenSettings(ui::screenSize() - available);
  int16_t x1, y1;
  uint16_t w, h;
  screen.getTextBounds(text, screen.getCursorX(), screen.getCursorY(), &x1, &y1, &w, &h);
  computedSize = vec2u(x1 + w - screen.getCursorX(), y1 + h - screen.getCursorY());
}

void Label::draw(vec2i offset) {
  setScreenSettings(offset);
  screen.print(text);
}

// ************************************************************** Image
void Image::draw(vec2i offset) {
  drawImage(offset, computedSize, image);
}
}