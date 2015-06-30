#ifndef VIGRIDR_SERVER_GAME_CONFIG
#define VIGRIDR_SERVER_GAME_CONFIG

#include <chrono>

#include "GameType.h"

namespace mjollnir { namespace vigridr { namespace config {

/**
 *  Time for the client to process the world model and send the command
 */
const std::chrono::milliseconds cycleDurationMs(1000);  // >= 3ms

/**
 *  Delay before the first cycle (for connection and initialization)
 */
const std::chrono::seconds firstCycleDurationS(4);

/**
 *  An upper bound on the time the server takes to update the world model
 */
const std::chrono::milliseconds updateTimeUpperBoundMs(10);

/**
 *  Type of the game (it can be either GameType::TURN or GameType::CONTINUOUS)
 */
const GameType gameType = GameType::CONTINUOUS;

}}}

#endif
