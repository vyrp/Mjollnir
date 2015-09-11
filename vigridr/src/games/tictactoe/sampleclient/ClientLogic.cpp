#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::Coordinate;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Marker;


std::ostream& operator<<(std::ostream& os, Coordinate c) {
    return os << "Coordinate(" << c.x << ", " << c.y << ")";
}

void init(const GameInit& gameInit) {
  srand(time(NULL));

  std::cout << "C++ Client" << std::endl;
  std::cout << "PlayerType: " << (gameInit.gameDescription.myType == Marker::X ? "X" : "O") << std::endl;
}

Command playTurn(const WorldModel& wm) {
  Command command;

  while(true) {
    size_t x = rand()%3;
    size_t y = rand()%3;
    if (wm.table[x][y] == Marker::UNMARKED) {
      command.coordinate.x = x;
      command.coordinate.y = y;
      break;
    }
  }

  std::cout << command.coordinate << std::endl;
  return command;
}

