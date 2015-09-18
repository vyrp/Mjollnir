#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Action;

Command playTurn(const WorldModel& wm) {
  Command command;
  size_t move = rand()%4;

  if(move == 0){
    std::cout << "FORWARD" << std::endl;
    command.action = Action::FORWARD;
  } else if(move == 1){
    std::cout << "TURNRIGHT" << std::endl;
    command.action = Action::TURNRIGHT;
  } else if(move == 2){
    std::cout << "TURNLEFT" << std::endl;
    command.action = Action::TURNLEFT;
  } else {
    std::cout << "STAY" << std::endl;
    command.action = Action::STAY;
  }
  return command;
}

void init(const GameInit& gameInit) {
  srand(time(NULL));
}
