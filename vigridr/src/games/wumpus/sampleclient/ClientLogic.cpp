#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Marker;


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
  return command;
}

void init(const GameInit& gameInit) {
  srand(time(NULL));
}
