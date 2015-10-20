#include "ClientLogic.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::Coordinate;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::Marker;

/*
 * A helper function to print a Coordinate on screen.
 */
std::ostream& operator<<(std::ostream& os, Coordinate c) {
    return os << "Coordinate(" << c.x << ", " << c.y << ")";
}

/*
 * This function is called at the beginning of the game.
 * You may do initialization here.
 *
 * Parameter:
 *     gameInit - contains a field named gameDescription, which itself contains a field named myType.
 *                myType is of type Marker, which is an enum. Marker has three fields: UNMARKED, X and O.
 */
void init(const GameInit& gameInit) {
  srand(time(NULL));

  std::cout << "C++ Client" << std::endl;
  std::cout << "PlayerType: " << (gameInit.gameDescription.myType == Marker::X ? "X" : "O") << std::endl;
}

/*
 * This function is called once for every turn.
 * This specific example solution returns a random valid position.
 *
 * Parameter:
 *     wm - an instance of the WorldModel class that contains a field called table which is a vector of vectors of Markers.
 *
 * Returns:
 *     A Command instance - a Command contains a field called coordinate of class Coordinate.
 *                          A Coordinate contains two fields of type int32_t, x and y.
 */
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

