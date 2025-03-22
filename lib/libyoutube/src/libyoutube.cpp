#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <nestegg.h>

namespace libyoutube {
String getVideoUrl = "https://inv.nadeko.net/latest_version";
// JsonDocument getVideoInfo(const char *videoId) {
//   const char *url = "https://www.youtube.com/youtubei/v1/"
//                     "player?key=AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8";
//   JsonDocument response;

//   WiFiClientSecure client;
//   client.setInsecure();

//   // clang-format off
//   String payload = String("{\"context\": {\"client\": {\"hl\": \"en\",
//   \"clientName\": \"WEB\", \"clientVersion\": \"2.20210721.00.00\",
//   \"clientFormFactor\": \"UNKNOWN_FORM_FACTOR\", \"clientScreen\": \"WATCH\",
//   \"mainAppWebInfo\": {\"graftUrl\": \"/watch?v=") + videoId + "\"}},
//   \"user\": {\"lockedSafetyMode\": false}, \"request\": {\"useSsl\": true,
//   \"internalExperimentFlags\": [], \"consistencyTokenJars\": []}},
//   \"videoId\": \"" + videoId + "\", \"playbackContext\":
//   {\"contentPlaybackContext\": {\"vis\": 0, \"splay\": false,
//   \"autoCaptionsDefaultOn\": false, \"autonavState\": \"STATE_NONE\",
//   \"html5Preference\": \"HTML5_PREF_WANTS\", \"lactMilliseconds\": \"-1\"}},
//   \"racyCheckOk\": false, \"contentCheckOk\": false}";
//   // clang-format on

//   HTTPClient http;
//   http.useHTTP10();
//   for (uint8_t retry = 0; retry < 1; retry++) {
//     if (http.begin(client, url)) {
//       http.addHeader("Content-Type", "application/json");
//       int httpCode = http.POST(payload);
//       if (httpCode <= 0) {
//         response["error"] = http.errorToString(httpCode);
//         break;
//       }

//       if (httpCode == HTTP_CODE_OK || httpCode ==
//       HTTP_CODE_MOVED_PERMANENTLY) {
//         response.clear();
//         deserializeJson(response, http.getStream());
//       } else {
//         response["code"] = httpCode;
//         response["error"] = http.getString();
//       }

//       break;
//     } else {
//       response["error"] = "Unable to connect";
//     }
//   }
//   http.end();

//   return response;
// }

struct NesteggStream {
  Stream *stream;
  size_t read = 0;
};

int nestegg_stream_read(void *buffer, size_t length, void *userdata) {
  NesteggStream *stream = (NesteggStream *)userdata;
  size_t read = stream->stream->readBytes((uint8_t *)buffer, length);
  stream->read += read;
  return read == length ? 1 : 0;
}

int nestegg_stream_seek(int64_t offset, int whence, void *userdata) {
  return -1;
}

int64_t nestegg_stream_tell(void *userdata) {
  NesteggStream *stream = (NesteggStream *)userdata;
  return stream->read;
}

void nestegg_log(nestegg *context, unsigned int severity, char const *format,
                 ...) {
  Serial.printf("[%u]", severity);
  va_list arg;
  va_start(arg, format);
  char temp[64];
  char *buffer = temp;
  size_t len = vsnprintf(temp, sizeof(temp), format, arg);
  va_end(arg);
  if (len > sizeof(temp) - 1) {
    buffer = new char[len + 1];
    if (!buffer) {
      return;
    }
    va_start(arg, format);
    vsnprintf(buffer, len + 1, format, arg);
    va_end(arg);
  }
  len = Serial.write((const uint8_t *)buffer, len);
  if (buffer != temp) {
    delete[] buffer;
  }
  Serial.println();
}

void getStream(const char *videoId, int itag) {
  const String url = getVideoUrl + "?id=" + videoId + "&itag=" + itag;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.useHTTP10(true);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  if (http.begin(client, url)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        NesteggStream stream;
        stream.stream = http.getStreamPtr();
        nestegg_io io{
            .read = nestegg_stream_read,
            .seek = nestegg_stream_seek,
            .tell = nestegg_stream_tell,
            .userdata = &stream,
        };

        nestegg *demux_ctx;
        nestegg_init(&demux_ctx, io, nestegg_log, -1);
        Serial.println("Init done.");

        nestegg_packet *pkt;
        int r;
        while ((r = nestegg_read_packet(demux_ctx, &pkt)) > 0) {
          Serial.printf("Packet: %p\n", pkt);
          unsigned int track;
          nestegg_packet_track(pkt, &track);
          Serial.printf("Track: %u\n", track);

          // This example decodes the first track only.
          if (track == 0) {
            unsigned int chunk, chunks;

            nestegg_packet_count(pkt, &chunks);

            // Decode each chunk of data.
            for (chunk = 0; chunk < chunks; ++chunk) {
              unsigned char *data;
              size_t data_size;

              nestegg_packet_data(pkt, chunk, &data, &data_size);
              Serial.printf("Got %zs\n", data_size);
            }
          }

          nestegg_free_packet(pkt);
        }

        Serial.println("Loop end.");

        nestegg_destroy(demux_ctx);

        // webm::Webm webm;
        // Serial.println(webmResult2string(
        //     webm::parseWebmHeader(http.getStreamPtr(), webm)));
        // Serial.println(webm.timecodeScale);
      }
    } else {
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  }
}
} // namespace libyoutube