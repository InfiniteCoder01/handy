#pragma once

template <typename... Args>
inline const String format(const String &format, Args... args) {
  int size = snprintf(NULL, 0, format.c_str(), args...);
  char buf[size + 1];
  sprintf(buf, format.c_str(), args...);
  return String(buf);
}