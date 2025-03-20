#include "ebml.h"

namespace ebml {
static WebmResult getVintSize(uint8_t szb, uint8_t &size) {
  if (szb & 0b1000'0000)
    size = 1;
  else if (szb & 0b0100'0000)
    size = 2;
  else if (szb & 0b0010'0000)
    size = 3;
  else if (szb & 0b0001'0000)
    size = 4;
  else if (szb & 0b0000'1000)
    size = 5;
  else if (szb & 0b0000'0100)
    size = 6;
  else if (szb & 0b0000'0010)
    size = 7;
  else if (szb & 0b0000'0001)
    size = 8;
  else
    return WebmResult::InvalidVint;
  return WebmResult::Ok;
}

template <typename T>
static WebmResult readVint(Stream *source, T &value, size_t &read) {
  uint8_t szb;
  if (source->readBytes(&szb, 1) < 1)
    return WebmResult::TimedOut;
  read++;

  uint8_t size;
  WEBM_TRY(getVintSize(szb, size));

  if (size > sizeof(T))
    return WebmResult::InvalidVint;

  uint8_t data[size];
  data[0] = szb & ((1 << (8 - size)) - 1);
  size_t actuallyRead = source->readBytes(data + 1, size - 1);
  if (actuallyRead != size - 1)
    return WebmResult::TimedOut;
  read += actuallyRead;

  value = (T)0;
  for (uint8_t i = 0; i < size; i++)
    value |= (T)data[i] << ((size - i - 1) * 8);
  return WebmResult::Ok;
}

static WebmResult _nextElement(Stream *source, Element &element, size_t &read) {
  WEBM_TRY(readVint(source, element.classId, read));
  WEBM_TRY(readVint(source, element.size, read));
  element.readBytes = 0;
  return WebmResult::Ok;
}

WebmResult nextElement(Stream *source, Element &element) {
  size_t read = 0;
  return _nextElement(source, element, read);
}

WebmResult Element::nextElement(Stream *source, Element &element) {
  if (readBytes >= size)
    return WebmResult::End;
  return _nextElement(source, element, readBytes);
}

WebmResult Element::skip(Stream *source) {
  while (readBytes < size) {
    uint8_t dummy;
    size_t read = source->readBytes(&dummy, 1);
    if (read < 1)
      return WebmResult::TimedOut;
    readBytes += read;
  }
  return WebmResult::Ok;
}

WebmResult Element::read(Stream *source, uint8_t *to) {
  size_t read = source->readBytes(to, size);
  if (read < size)
    return WebmResult::TimedOut;
  readBytes += read;
  return WebmResult::Ok;
}
} // namespace ebml