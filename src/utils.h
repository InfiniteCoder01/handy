#pragma once

template<typename... Args> inline const String format(const String& format, Args... args) {
  char buf[256];
  sprintf(buf, format.c_str(), args...);
  return String(buf);
}