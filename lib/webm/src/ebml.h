#pragma once
#include "webm_result.h"
#include <Arduino.h>
#include <stdint.h>

/// @brief Streaming EBML (Extensible Binary Markup Language, format used for
/// matroska and webm) parser
namespace ebml {
/// @brief An EBML element header.
struct Element {
  uint64_t classId;
  uint64_t size;
  size_t readBytes = 0;

  /// @brief Read element inside of this element from Arduino stream. Returns
  /// WebmResult::End once end
  /// @param source Arduino stream
  /// @param element target element
  /// @return WebmResult
  WebmResult nextElement(Stream *source, Element &element);

  /// @brief Skip this element
  /// @param source Arduino stream
  /// @return WebmResult
  WebmResult skip(Stream *source);

  /// @brief Read contents of this element
  /// @param to pointer to the value buffer (must be allocated and sized
  /// accordingly to size of this element)
  /// @param source Arduino stream
  /// @return WebmResult
  WebmResult read(Stream *source, uint8_t *to);

  /// @brief Read contents of this element to a varaible of concrete type
  /// @tparam T The type
  /// @param source Arduino stream
  /// @param to Read target
  /// @return WebmResult
  template <typename T> WebmResult readTo(Stream *source, T &to) {
    if (sizeof(T) != size)
      return WebmResult::InvalidElementSize;
    return read(source, (uint8_t *)&to);
  }

  /// @brief Read contents of this element into an int, with left padding
  /// @tparam T type
  /// @param source Arduino stream
  /// @param to Read target
  /// @return WebmResult
  template <typename T> WebmResult readInt(Stream *source, T &to) {
    if (sizeof(T) < size)
      return WebmResult::InvalidElementSize;
    to = (T)0;
    uint8_t buf[size];
    WEBM_TRY(read(source, buf));
    for (uint8_t i = 0; i < size; i++)
      to <<= 8, to |= buf[i];
    return WebmResult::Ok;
  }
};

/// @brief Read element header from Arduino stream
/// @param source Arduino stream
/// @param element target element
/// @return WebmResult
WebmResult nextElement(Stream *source, Element &element);
} // namespace ebml