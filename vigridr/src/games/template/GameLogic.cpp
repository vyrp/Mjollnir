#include "GameLogic.h"
#include <iostream>
#include <stdio.h>

namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  player1_ = playerId1;
  player2_ = playerId2;
  winner_ = kNoWinner;
  hasFinished_ = false;
  // TODO: implement initialization logic
}

bool GameLogic::update(Command command, int32_t playerId) {
  // TODO: implement update logic
  return true;
}

WorldModel GameLogic::getWorldModel() const {
  return worldModel_;
}

bool GameLogic::isFinished() const {
  return hasFinished_;
}
  
int32_t GameLogic::getWinner() const {
  return winner_;
}

GameDescription GameLogic::getGameDescription(int32_t playerId) const {
  GameDescription gameDescription;
  // TODO: implement game description logic
  return gameDescription;
}

TotalWorldModel GameLogic::getTotalWorldModel() const {
  return twm_;
}

}}  // namespaces