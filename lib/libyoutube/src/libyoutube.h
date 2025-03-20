#pragma once

namespace libyoutube {
// "https://inv.nadeko.net/latest_version" by default.
// We use it to get video URL without needing to decrypt youtube's cypher
extern String getVideoUrl;

void getStream(const char *videoId, int itag);
}