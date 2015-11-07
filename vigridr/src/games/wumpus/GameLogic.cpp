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
  winner_ = "s:-2000";
  score_ = 0;
  playerPosition_.x = worldSize_ - 1;
  playerPosition_.y = 0;
  hasFinished_ = false;
  canShoot_ = true;
  hasGold_ = false;
  srand(time(NULL));

  //initializeWorld_();
  initializeWorldFromRusselBook_();
}

void GameLogic::initializeWorld(std::vector<std::vector<WorldSquare>> map) {

  twm_.map = map;

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

void GameLogic::initializeWorld_(){
  int32_t x, y;

  // Creating world map
  for(size_t i = 0; i < worldSize_; i++) {
    twm_.map.push_back(std::vector<WorldSquare>(worldSize_, WorldSquare()));
  }

  // Filling map with pits: 20% of chance of having a pit 
  // in a position different from the initial position
  for(size_t i = 0; i < worldSize_; i++) {
    for(size_t j = 0; j < worldSize_; j++) {
      if((int32_t)i != playerPosition_.x || (int32_t)j != playerPosition_.y) {
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

void GameLogic::initializeWorldFromRusselBook_(){
  // Creating world map
  for(size_t i = 0; i < worldSize_; i++) {
    twm_.map.push_back(std::vector<WorldSquare>(worldSize_, WorldSquare()));
  }

  // Filling map with pits
  twm_.map[3][2].pit = true;
  twm_.map[1][2].pit = true;
  twm_.map[0][3].pit = true;

  // Placing Wumpus
  twm_.map[1][0].wumpus = true;
  wumpusPosition_.x = 1;
  wumpusPosition_.y = 0;

  // Placing gold
  twm_.map[1][1].gold = true;

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
  if(twm_.map[x][y].wumpus){
    twm_.map[x][y].stench = true;
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
      worldModel_.sensors.scream = false;
      bump = checkBump_(i, j);
      if(bump){
        worldModel_.sensors.bump = true;
      } else {
        twm_.map[playerPosition_.x][playerPosition_.y].player = false;
        resetSensors_();
        playerPosition_.x = i;
        playerPosition_.y = j;
        twm_.map[playerPosition_.x][playerPosition_.y].player = true;

        if((twm_.map[playerPosition_.x][playerPosition_.y].wumpus && wumpusAlive_) || 
            twm_.map[playerPosition_.x][playerPosition_.y].pit)  {
          hasFinished_ = true;
          winner_ = "s:" + std::to_string(score_ - 1000);
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
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
      score_--;
      facing_ = (Direction)(((int32_t)facing_ + 1)%4);
      // facing_ = (facing_ + 1)%4;
      twm_.playerDirection = facing_;
      break;
    case TURNLEFT:
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
      score_--;
      facing_ = (Direction)((int32_t)facing_ - 1);
      // facing_ = facing_ - 1;
      if(facing_ == -1) {
        facing_ = LEFT;
      }
      twm_.playerDirection = facing_;
      break;  
    case SHOOT:
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
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
        if (!wumpusAlive_) {
          twm_.map[wumpusPosition_.x][wumpusPosition_.y].wumpus = false;
        }
        canShoot_ = false;
      } else {
        score_--;
      }
      break;
    case GRAB:
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
      score_--;
      if(twm_.map[playerPosition_.x][playerPosition_.y].gold){
        hasGold_ = true;
        twm_.map[playerPosition_.x][playerPosition_.y].gold = false;
        worldModel_.sensors.glitter = false;
      }
      break;
    case CLIMB:
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
      if(playerPosition_.x == (int32_t)worldSize_ - 1 && playerPosition_.y == 0){
        if(hasGold_){
          score_ += 1000;
        } 
        score_--;
        winner_ = "s:" + std::to_string(score_);
        hasFinished_ = true;
      } else {
        score_--;
      }
      break;
    default:
      score_--;
      worldModel_.sensors.scream = false;
      worldModel_.sensors.bump = false;
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

bool GameLogic::shouldPrintWorldModel(int32_t playerId){
  return playerId == player1_;
}

bool GameLogic::shouldIncrementCycle(int32_t playerId){
  return playerId == player1_;
}

size_t GameLogic::getNumberOfPlayers() const {
  return numberOfPlayers_;
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

size_t GameLogic::getWorldSize() const {
  return worldSize_;
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

void GameLogic::setWumpusPosition(int32_t x, int32_t y){
  wumpusPosition_.x = x;
  wumpusPosition_.y = y;
}

GameResult GameLogic::createGameResult(std::string result, int32_t id) {
  GameResult gr;
  if (result[0] == 's' && result[1] == ':') {
    gr.score = std::stoi(result.substr(2));
  } else {
    gr.score = -3000;
  }
  return gr;
}

}}  // namespaces

