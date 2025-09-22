#pragma once

template <typename... Args>
inline const String format(const char *fmt, Args... args) {
  int size = snprintf(NULL, 0, fmt, args...);
  char buf[size + 1];
  sprintf(buf, fmt, args...);
  return String(buf);
}