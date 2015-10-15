#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Action;

Command playTurn(const WorldModel& wm) {
  Command command;
  size_t move = rand()%3;

  if(move == 0){
    std::cout << "FORWARD" << std::endl;
    command.action = Action::FORWARD;
  } else if(move == 1){
    std::cout << "TURNRIGHT" << std::endl;
    command.action = Action::TURNRIGHT;
  } else {
    std::cout << "TURNLEFT" << std::endl;
    command.action = Action::TURNLEFT;
  } 
  
  return command;
}

void init(const GameInit& gameInit) {
  srand(time(NULL));
}
