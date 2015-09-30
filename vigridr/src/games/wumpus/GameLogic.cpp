#include "GameLogic.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  player1_ = playerId1;
  player2_ = playerId2;
  facing_ = RIGHT;
  wumpusAlive_ = true;
  winner_ = "-1";
  score_ = 0;
  playerPosition_.x = worldSize_ - 1;
  playerPosition_.y = 0;
  hasFinished_ = false;
  canShoot_ = true;
  hasGold_ = false;
  srand(time(NULL));

  initializeWorld_();
}

void GameLogic::initializeWorld_(){
  int32_t x, y;

  // Creating world map
  for(size_t i = 0; i < worldSize_; i++) {
    std::vector<WorldSquare> row;
    for(size_t j = 0; j < worldSize_; j++) {
      WorldSquare ws;
      row.push_back(ws);
    }
    twm_.map.push_back(row);
  }

  // Filling map with pits: 20% of chance of having a pit 
  // in a position different from the initial position
  for(size_t i = 0; i < worldSize_; i++) {
    for(size_t j = 0; j < worldSize_; j++) {
      if(i != 0 || j != 0) {
        if(rand() % 10 < 2){
          twm_.map[i][j].pit = true;
        }
      }
    }
  }

  // Choosing Wumpus position
  do {
    x = rand() % worldSize_;
    y = rand() % worldSize_;
  } while (x == playerPosition_.x && y == playerPosition_.y);
  twm_.map[x][y].wumpus = true;
  wumpusPosition_.x = x;
  wumpusPosition_.y = y;

  // Choosing gold position different from pit and initial position
  do {
    x = rand() % worldSize_;
    y = rand() % worldSize_;
  } while ((x == playerPosition_.x && y == playerPosition_.y) || twm_.map[x][y].pit);
  twm_.map[x][y].gold = true;

  for(int32_t i = 0; i < (int32_t)worldSize_; i++) {
    for(int32_t j = 0; j < (int32_t)worldSize_; j++) {
      updateWorldSquare_(i, j);
    }
  }

  twm_.playerDirection = facing_;
  twm_.map[playerPosition_.x][playerPosition_.y].player = true;
  worldModel_.sensors.stench = twm_.map[worldSize_ -1][0].stench;
  worldModel_.sensors.breeze = twm_.map[worldSize_ -1][0].breeze;
}

bool GameLogic::update(Command command, int32_t playerId) {
  if(!hasFinished_) {
    if (playerId == player2_) {
      // Do nothing or move wumpus
    }
    else if (playerId == player1_) {
      updateWorldModel_(command.action);
    }
    return true;
  }
  return false;
}

bool GameLogic::checkBump_(int32_t x, int32_t y){
  if(x < 0 || y < 0 || x == (int32_t)worldSize_ || y == (int32_t)worldSize_){
    return true;
  }
  return false;
}

void GameLogic::updateWorldSquare_(int32_t x, int32_t y){
  if(x > 0) {
    updateSensors_(twm_.map[x][y], x - 1, y);
  }
  if(y > 0) {
    updateSensors_(twm_.map[x][y], x, y - 1);
  }
  if(x < (int32_t)worldSize_ - 1){
    updateSensors_(twm_.map[x][y], x + 1, y);
  }
  if(y < (int32_t)worldSize_ - 1){
    updateSensors_(twm_.map[x][y], x, y + 1); 
  }
}

void GameLogic::updateSensors_(WorldSquare& ws, int32_t x, int32_t y){
  if(twm_.map[x][y].pit){
    ws.breeze = true;
  }
  if(twm_.map[x][y].wumpus){
    ws.stench = true;
  }
}

void GameLogic::updateWorldModel_(Action action){
  int32_t i, j;
  bool bump;
  i = playerPosition_.x;
  j = playerPosition_.y;
  switch(action){
    case FORWARD:
      score_--;
      switch(facing_){
        case UP:
          i--;
          break;
        case DOWN:
          i++;
          break;
        case LEFT:
          j--;
          break;
        case RIGHT:
          j++;
          break;
      }
      bump = checkBump_(i, j);
      if(bump){
        worldModel_.sensors.bump = true;
      } else {
        twm_.map[playerPosition_.x][playerPosition_.y].player = false;
        resetSensors_();
        playerPosition_.x = i;
        playerPosition_.y = j;
        twm_.map[playerPosition_.x][playerPosition_.y].player = true;
        if((twm_.map[playerPosition_.x][playerPosition_.y].wumpus) || 
            twm_.map[playerPosition_.x][playerPosition_.y].pit)  {
          hasFinished_ = true;
          winner_ = "s:" + std::to_string(score_);
        } else {
          if(twm_.map[playerPosition_.x][playerPosition_.y].gold){
            worldModel_.sensors.glitter = true;          
          }
          worldModel_.sensors.breeze = twm_.map[playerPosition_.x][playerPosition_.y].breeze;
          worldModel_.sensors.stench = twm_.map[playerPosition_.x][playerPosition_.y].stench;
        }
      }
      
      break;
    case TURNRIGHT:
      score_--;
      facing_ = (Direction)(((int32_t)facing_ + 1)%4);
      // facing_ = (facing_ + 1)%4;
      twm_.playerDirection = facing_;
      break;
    case TURNLEFT:
      score_--;
      facing_ = (Direction)((int32_t)facing_ - 1);
      // facing_ = facing_ - 1;
      if(facing_ == -1) {
        facing_ = LEFT;
      }
      twm_.playerDirection = facing_;
      break;  
    case SHOOT:
      if(canShoot_){
        score_-=10;
        switch(facing_) {
          case UP:
            if(wumpusPosition_.y == playerPosition_.y && wumpusPosition_.x < playerPosition_.x){
              worldModel_.sensors.scream = true;
              wumpusAlive_ = false;
            }
            break;
          case RIGHT:
            if(wumpusPosition_.x == playerPosition_.x && wumpusPosition_.y > playerPosition_.y){
              worldModel_.sensors.scream = true;
              wumpusAlive_ = false;
            }
            break;
          case DOWN:
            if(wumpusPosition_.y == playerPosition_.y && wumpusPosition_.x > playerPosition_.x){
              worldModel_.sensors.scream = true;
              wumpusAlive_ = false;
            }
            break;
          case LEFT:
            if(wumpusPosition_.x == playerPosition_.x && wumpusPosition_.y < playerPosition_.y){
              worldModel_.sensors.scream = true;
              wumpusAlive_ = false;
            }
            break;
        }
        canShoot_ = false;
      } else {
        score_--;
      }
      break;
    case GRAB:
      score_--;
      if(twm_.map[playerPosition_.x][playerPosition_.y].gold){
        hasGold_ = true;
      }
      break;
    case CLIMB:
      if(playerPosition_.x == 0 && playerPosition_.y == 0){
        if(hasGold_){
          score_ += 1000;
        } else {
          score_--;
        }
        winner_ = "s:" + std::to_string(score_);
        hasFinished_ = true;
      } else {
        score_--;
      }
      break;
    default:
      score_--;
      break;
  }
}

void GameLogic::resetSensors_(){
  worldModel_.sensors.breeze = false;
  worldModel_.sensors.stench = false;
  worldModel_.sensors.glitter = false;
  worldModel_.sensors.bump = false;
  worldModel_.sensors.scream = false;
}

GameDescription GameLogic::getGameDescription(int32_t playerId) const {
  GameDescription gameDescription;
  gameDescription.playerType = (playerId == player1_) ? PLAYER : COMPUTER;
  return gameDescription;
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

bool GameLogic::randomPlay_(int32_t playerId) {
  return true;
}

TotalWorldModel GameLogic::getTotalWorldModel() const {
  return twm_;
}

}}  // namespaces