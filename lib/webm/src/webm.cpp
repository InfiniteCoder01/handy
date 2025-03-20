#include "webm.h"
#include "ebml.h"

namespace webm {
template <typename T> static WebmResult readVal(Stream *stream, T &val) {
  size_t read = stream->readBytes(&val, sizeof(val));
  if (read < sizeof(val))
    return WebmResult::TimedOut;
  return WebmResult::Ok;
}

static WebmResult skip(Stream *stream, size_t size) {
  while (size > 0) {
    uint8_t tmp;
    WEBM_TRY(readVal(stream, tmp));
    size--;
  }
  return WebmResult::Ok;
}

// static WebmResult decodeTrackMeta(Stream *stream, size_t elementSize,
//                                   Track &track) {
//   ebml::Element element;
//   size_t read = 0;
//   while (read < elementSize) {
//     WEBM_TRY(ebml::nextElement(stream, element, &read));
//     if (element.classId == 0x57)
//       WEBM_TRY(readVal(stream, track.trackNumber));
//     else if (element.classId == 0x03)
//       WEBM_TRY(readVal(stream, track.trackType));
//     else if (element.classId == 0x2b59c) {
//       char language[element.size + 1];
//       if (stream->readBytes(language, element.size) < element.size)
//         return WebmResult::TimedOut;
//       language[element.size] = '\0';
//       track.language = language;
//     } else if (element.classId == 0x06) {
//       char codec[element.size + 1];
//       if (stream->readBytes(codec, element.size) < element.size)
//         return WebmResult::TimedOut;
//       codec[element.size] = '\0';
//       track.codecID = codec;
//     } else if (element.classId == 0x61) {
//       // TODO: Audio Track
//     } else
//       WEBM_TRY(skip(stream, element.size));
//     read += element.size;
//   }
// }

WebmResult parseWebmHeader(Stream *stream, Webm &webm) {
  ebml::Element segment;

  // Find "Segment" element
  while (true) {
    WEBM_TRY(ebml::nextElement(stream, segment));
    if (segment.classId != 0x8538067)
      WEBM_TRY(segment.skip(stream))
    else
      break;
  }

  ebml::Element element;
  while (true) {
    WEBM_TRY_BREAK(segment.nextElement(stream, element));
    if (element.classId == 0x549a966) {
      // Segment info
      ebml::Element segmentInfo = element;
      while (true) {
        WEBM_TRY_BREAK(segmentInfo.nextElement(stream, element));
        if (element.classId == 0xad7b1)
          WEBM_TRY(element.readInt(stream, webm.timecodeScale))
        else
          WEBM_TRY(element.skip(stream));
      }
      break;
    } else if (element.classId == 0x654ae6b) {
      // TODO: Tracks
    } else
      WEBM_TRY(element.skip(stream));
  }
  return WebmResult::Ok;
}
} // namespace webm
