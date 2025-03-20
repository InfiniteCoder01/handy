#pragma once

#define WEBM_TRY(expr)                                                         \
  {                                                                            \
    WebmResult _result = expr;                                                 \
    if (_result != WebmResult::Ok)                                             \
      return _result;                                                          \
  }

#define WEBM_TRY_BREAK(expr)                                                   \
  {                                                                            \
    WebmResult _result = expr;                                                 \
    if (_result == WebmResult::End)                                            \
      break;                                                                   \
    if (_result != WebmResult::Ok)                                             \
      return _result;                                                          \
  }

/// @brief Result type, used as the return type for functions which might fail
enum class WebmResult {
  /// @brief Success
  Ok,
  /// @brief End of iteration
  End,
  /// @brief Invalid variable-sized int
  InvalidVint,
  /// @brief Invalid element size
  InvalidElementSize,
  /// @brief Stream timed out
  TimedOut,
};

/// @brief Convert WebmResult to human-readable string
/// @param e WebmResult
/// @return C-String, human readable representation of the result
const char *webmResult2string(WebmResult e);