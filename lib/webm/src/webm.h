#pragma once
#include "webm_result.h"
#include <Arduino.h>

namespace webm {
struct Track {
  uint8_t trackNumber;
  enum Type : uint8_t {
    TYPE_VIDEO = 1,
    TYPE_AUDIO = 2,
    TYPE_COMPLEX = 3,
    TYPE_LOGO = 0x10,
    TYPE_SUBTITLE = 0x11,
    TYPE_BUTTONS = 0x12,
    TYPE_CONTROL = 0x20,
  } trackType;
  String language;
  String codecID;

  union {
    struct {
      uint32_t samplingFrequency;
      uint8_t channels;
      uint8_t bitDepth;
    } audioTrack;
  };
};

struct Webm {
  uint64_t timecodeScale;
};

/// @brief Parse webm header from stream and get ready for reading track data
/// @param stream Arduino stream
/// @param webm Webm header to write data to
/// @return WebmResult
WebmResult parseWebmHeader(Stream *stream, Webm &webm);
} // namespace webm