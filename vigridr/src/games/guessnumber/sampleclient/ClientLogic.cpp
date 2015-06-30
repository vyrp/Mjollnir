#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;

Command playTurn(const WorldModel& wm) {
  Command command;
  command.number = rand()%10 + 1;
  return command;
}

void init(const GameInit& gameInit) {
  srand(time(NULL));
}
