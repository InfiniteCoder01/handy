#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <webm.h>

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
        webm::Webm webm;
        Serial.println(webmResult2string(
            webm::parseWebmHeader(http.getStreamPtr(), webm)));
        Serial.println(webm.timecodeScale);
      }
    } else {
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  }
}
} // namespace libyoutube