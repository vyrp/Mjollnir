#include "GameLogic.h"
#include <iostream>
#include <stdio.h>

namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  player1_ = playerId1;
  player2_ = playerId2;
  srand(time(NULL));
  winner_ = "-1";
  hasFinished_ = false;
  worldModel_.target = rand()%maxValue_ + 1;
}

bool GameLogic::update(Command command, int32_t playerId) {
  if(!hasFinished_) {
    worldModel_.guess = command.number;
    hasFinished_ = checkVictory_(command, playerId);
    return true;
  }
  return false;
}

GameDescription GameLogic::getGameDescription(int32_t playerId) const {
  GameDescription gameDescription;
  gameDescription.targetNumber = worldModel_.target;
  return gameDescription;
}

bool GameLogic::randomPlay_(int32_t playerId) {
  // for(auto& line : worldModel_.table)
  //   for(auto& element : line)
  //     if(element == Marker::UNMARKED) {
  //       if(playerId == player1_) {
  //         element = Marker::X;
  //         return true;
  //       }
  //       else if(playerId == player2_) {
  //         element = Marker::O;
  //         return true;
  //       }
  //     }
  // return false;
  return true;
}

bool GameLogic::checkVictory_(Command command, int32_t playerId) {
  if(command.number == worldModel_.target) {
    winner_ = std::to_string(playerId);
    return true;
  }
  return false;
}

WorldModel GameLogic::getWorldModel() const {
  return worldModel_;
}

bool GameLogic::isFinished() const {
  return hasFinished_;
}

void GameLogic::setHasFinished(bool value) {
  hasFinished_ = value;
}
  
std::string GameLogic::getWinner() const {
  return winner_;
}

void GameLogic::setWinner(std::string value) {
  winner_ = value;
}

TotalWorldModel GameLogic::getTotalWorldModel() const {
  return twm_;
}

}}  // namespaces