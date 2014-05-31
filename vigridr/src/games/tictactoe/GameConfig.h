#ifndef VIGRIDR_SERVER_GAME_CONFIG
#define VIGRIDR_SERVER_GAME_CONFIG

#include <chrono>

#include "GameType.h"

namespace mjollnir { namespace vigridr { namespace config {
const std::chrono::milliseconds cycleDurationMs(20);  // >= 3ms
const std::chrono::seconds firstCycleDurationS(4);
const std::chrono::milliseconds updateTimeUpperBoundMs(2);
const GameType gameType = GameType::TURN;
}}}

#endif
