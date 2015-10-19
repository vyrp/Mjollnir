#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Action;

/*
 * This function is called at the beginning of the game.
 * You may do initialization here.
 *
 * Parameter:
 *     gameInit - not used for Wumpus
 */
void init(const GameInit& gameInit) {
  srand(time(NULL));
  std::cout << "C++ Client" << std::endl;
}

/*
 * This function is called once for every turn.
 * This specific example solution returns a random action.
 *
 * Parameter:
 *     wm - an instance of the WorldModel class that contains a field called sensors of class Sensors.
 *          Sensors contains the boolean fields: breeze, stench, glitter, bump and scream.
 *
 * Returns:
 *     A Command instance - a Command contains a field called action of enum Action.
 *                          Action fields: FORWARD, TURNRIGHT, TURNLEFT, STAY, SHOOT, GRAB and CLIMB.
 */
Command playTurn(const WorldModel& wm) {
  Command command;
  int move = rand()%3;

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

