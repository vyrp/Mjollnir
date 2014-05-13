#include "GameTimer.h"

#include <thread>

#include "GameConfig.h"

namespace mjollnir { namespace vigridr {

/**
 *  Constants to synchronize the updating time so that the new world model
 *  is available exactely when promissed.
 */
constexpr std::chrono::milliseconds kWorldModelUpdateMsStep1(2);
constexpr std::chrono::milliseconds kWorldModelUpdateMsStep2(1);

void GameTimer::startCycle() {
  nextUpdateTime_ = clock::now() + config::cycleDurationMs;
  nextWorldModelTime_ = nextUpdateTime_ + config::updateTimeUpperBoundMs;
}

void GameTimer::startFirstCycle() {
  nextUpdateTime_ = clock::now() + config::firstCycleDurationS;
  nextWorldModelTime_ = nextUpdateTime_ + config::updateTimeUpperBoundMs;
}

void GameTimer::sleepUntilWorldModelUpdateTime() {
  std::this_thread::sleep_until(nextWorldModelTime_ - kWorldModelUpdateMsStep1);
}

void GameTimer::sleepUntilWorldModelTime() {
  std::this_thread::sleep_until(nextWorldModelTime_);
}

void GameTimer::sleepUntilPlayerUpdateTime() {
  std::this_thread::sleep_until(nextUpdateTime_);
}

int32_t GameTimer:: getWorldModelTime() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    nextWorldModelTime_-clock::now()).count();
}

int32_t GameTimer::getPlayerUpdateTime() { 
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    nextUpdateTime_-clock::now()).count();
}

}}