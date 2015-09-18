#ifndef VIGRIDR_SERVER_LOGGER
#define VIGRIDR_SERVER_LOGGER

#include <memory>
#include <algorithm>

#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"
#include "GameLogic.h"

namespace mjollnir { namespace vigridr {

class GameLogger {
 public:
  static void logWorldModel(const WorldModel& wm, const TotalWorldModel& twm);
  static void logGameDescription(const GameDescription& description1,
                                 const std::string& player1,
                                 const GameDescription& description2,
                                 const std::string& player2);
  static void flushLog();
};

}}

#endif  // VIGRIDR_SERVER_LOGGER
