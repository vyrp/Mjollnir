#ifndef VIGRIDR_SERVER_GAME_CONFIG
#define VIGRIDR_SERVER_GAME_CONFIG

#include <chrono>

#include "GameType.h"

namespace mjollnir { namespace vigridr { namespace config {

/**
 *  Time for the client to proccess the world model and send the command
 */
constexpr std::chrono::milliseconds cycleDurationMs(1000);  // >= 3ms

/**
 *  Delay before the first cycle (for connection and initialization)
 */
constexpr std::chrono::seconds firstCicleDurationS(4);

/** 
 *  An upper bound on the time the server takes to update the world model
 */
constexpr std::chrono::milliseconds updateTimeUpperBoundMs(10);

/**
 *  Type of the game (it can be either GameType::TURN or GameType::CONTINUOUS)
 */
constexpr GameType gameType = GameType::CONTINUOUS;

}}}

#endif