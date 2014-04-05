#ifndef VIGRIDR_SERVER_GAME_TIMER_H
#define VIGRIDR_SERVER_GAME_TIMER_H

#include <chrono>

namespace mjollnir { namespace vigridr {

typedef std::chrono::high_resolution_clock clock;
typedef std::chrono::time_point<clock> time_point;

class GameTimer {
 public:
  void startFirstCycle();
  void startCycle();
  void sleepUntilWorldModelUpdateTime();
  void sleepUntilWorldModelTime();
  void sleepUntilPlayerUpdateTime();
  int32_t getWorldModelTime();
  int32_t getPlayerUpdateTime();


 private:
  time_point nextUpdateTime_;
  time_point nextWorldModelTime_;
};


}}

#endif  // VIGRIDR_SERVER_GAME_TIMER_H