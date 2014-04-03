#ifndef VIGRIDR_SERVER_GAME_CONFIG
#define VIGRIDR_SERVER_GAME_CONFIG

#include <chrono>

#include "GameType.h"

namespace mjollnir { namespace vigridr { namespace config {
constexpr std::chrono::milliseconds cycleWaitMs(3);
constexpr std::chrono::seconds firstCicleWaitS(10);
constexpr std::chrono::milliseconds updateTimeUpperBoundMs(1);
constexpr GameType gameType = GameType::TURN;
}}}

#endif