#ifndef VIGRIDR_SERVER_LOGGER
#define VIGRIDR_SERVER_LOGGER

#include <memory>
#include <algorithm>

#include "../thrifts/gen-cpp/WorldModel_types.h"

namespace mjollnir { namespace vigridr {

class GameLogger {
 public:
  static void logWorldModel(const WorldModel& wm);
  static void flushLog();
};

}}

#endif  // VIGRIDR_SERVER_LOGGER
