#include "GameLogic.h"

#include <iostream>
#include <stdio.h>

namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  player1 = playerId1;
  player2 = playerId2;
  std::vector<std::vector<Marker> > table {
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED},
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED},
      {Marker::UNMARKED,Marker::UNMARKED,Marker::UNMARKED}
  };
  worldModel.table = table;
}

bool GameLogic::update(Command command, int32_t playerId) {
  printf("Updating... player %d\n", playerId);
  if(worldModel.table[command.coordinate.x][command.coordinate.y] == 
        Marker::UNMARKED) {
    if (playerId == player1) {
      worldModel.table[command.coordinate.x][command.coordinate.y] = 
        Marker::X;
      if(checkLines(worldModel, Marker::X) || 
         checkColumns(worldModel, Marker::X) ||
         checkDiagonals(worldModel, Marker::X) )
        std::cout << "Player X has won!" << std::endl;
    }
    else if (playerId == player2) {
      worldModel.table[command.coordinate.x][command.coordinate.y] = 
        Marker::O;
      if(checkLines(worldModel, Marker::O) || 
         checkColumns(worldModel, Marker::O) ||
         checkDiagonals(worldModel, Marker::O) )
        std::cout << "Player O has won!" << std::endl;
    }
    return true;
  }
  return false;
}

WorldModel GameLogic::getWorldModel() {
  // printf("worldModel\n");
  return worldModel;
}

bool GameLogic::checkLines(WorldModel wm, Marker player) {
  for(const auto& line : wm.table) {
    for(size_t i = 0; i < 3; ++i) {
      if(line[i] != player) break;
      if(i == line.size()-1) return true;
    }
  }
  return false;
}

bool GameLogic::checkColumns(WorldModel wm, Marker player) {
  for(size_t j = 0; j < 3; ++j) {
    for(size_t i = 0; i < 3; ++i) {
      if(wm.table[i][j] != player) break;
      if(i == wm.table[i].size()-1) return true;
    }
  }
  return false;
}

bool GameLogic::checkDiagonals(WorldModel wm, Marker player) {
  for(size_t i = 0; i < 3; ++i) {
    if(wm.table[i][2-i] != player) break;
    if(i == 2) return true;
  }
  for(size_t i = 0; i < 3; ++i) {
    if(wm.table[i][i] != player) break;
    if(i == 2) return true;
  }
  return false;
}

}}