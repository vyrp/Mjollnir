#include "GameLogic.h"

#include <iostream>
#include <stdio.h>

namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  player1 = playerId1;
  player2 = playerId2;
  winner = -1;
  hasFinished = false;
  std::vector<std::vector<Marker> > table {
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED},
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED},
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED}
  };
  worldModel.table = table;
}

bool GameLogic::update(Command command, int32_t playerId) {
  printf("Updating... player %d\n", playerId);
  if(checkTableCoordinate(command.coordinate, Marker::UNMARKED)) {
    if (playerId == player1) {
      setTableCoordinate(command.coordinate, Marker::X);
      if(!hasFinished && checkVictory(worldModel, Marker::X , playerId) )
        std::cout << "Player X has won!" << std::endl;
    }
    else if (playerId == player2) {
      setTableCoordinate(command.coordinate, Marker::O);
      if(!hasFinished && checkVictory(worldModel, Marker::O , playerId) )
        std::cout << "Player O has won!" << std::endl;
    }
    return true;
  }
  return false;
}

WorldModel GameLogic::getWorldModel() {
  return worldModel;
}

void GameLogic::setTableCoordinate(const Coordinate& coordinate, Marker marker) {
  worldModel.table[coordinate.x][coordinate.y] = marker;
}

bool GameLogic::checkTableCoordinate(const Coordinate& coordinate, Marker marker) {
  return worldModel.table[coordinate.x][coordinate.y] == marker;
}

bool GameLogic::isFinished() {
  return hasFinished;
}
  
int32_t GameLogic::getWinner() {
  return winner;
}

bool GameLogic::randomPlay(int32_t playerId) {
  for(auto& line : worldModel.table)
    for(auto& element : line)
      if(element == Marker::UNMARKED) {
        if(playerId == player1) {
          element = Marker::X; 
          return true;
        }
        else if(playerId == player2) {
          element = Marker::O;
          return true;
        }
      }
  return false;
}

bool GameLogic::checkLines(const WorldModel& wm, Marker player) {
  for(const auto& line : wm.table) {
    for(size_t i = 0; i < 3; ++i) {
      if(line[i] != player) break;
      if(i == line.size()-1) return true;
    }
  }
  return false;
}

bool GameLogic::checkColumns(const WorldModel& wm, Marker player) {
  for(size_t j = 0; j < boardSize; ++j) {
    for(size_t i = 0; i < boardSize; ++i) {
      if(wm.table[i][j] != player) break;
      if(i == boardSize-1) return true;
    }
  }
  return false;
}

bool GameLogic::checkDiagonals(const WorldModel& wm, Marker player) {
  for(size_t i = 0; i < boardSize; ++i) {
    if(wm.table[i][boardSize-1-i] != player) break;
    if(i == boardSize-1) return true;
  }
  for(size_t i = 0; i < boardSize; ++i) {
    if(wm.table[i][i] != player) break;
    if(i == boardSize-1) return true;
  }
  return false;
}

bool GameLogic::checkVictory(const WorldModel& wm, Marker player, int32_t playerId) {
  if(checkLines(worldModel, player) || 
     checkColumns(worldModel, player) ||
     checkDiagonals(worldModel, player) ) {
    winner = playerId;
    hasFinished = true;
  }
  return hasFinished;
}

}}