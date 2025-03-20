#include "webm_result.h"

const char *webmResult2string(WebmResult e) {
  if (e == WebmResult::Ok)
    return "Ok";
  else if (e == WebmResult::End)
    return "End of iteration";
  else if (e == WebmResult::InvalidVint)
    return "Invalid variable-sized integer";
  else if (e == WebmResult::InvalidElementSize)
    return "Invalid element size";
  else if (e == WebmResult::TimedOut)
    return "Timed out while parsing";
  return "Unknown";
}