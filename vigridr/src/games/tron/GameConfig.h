#ifndef VIGRIDR_SERVER_GAME_CONFIG
#define VIGRIDR_SERVER_GAME_CONFIG

#include <chrono>

#include "GameType.h"

namespace mjollnir { namespace vigridr { namespace config {
constexpr std::chrono::milliseconds cycleDurationMs(1000);  // >= 3ms
constexpr std::chrono::seconds firstCycleDurationS(4);
constexpr std::chrono::milliseconds updateTimeUpperBoundMs(10);
constexpr GameType gameType = GameType::CONTINUOUS;
}}}

#endif
