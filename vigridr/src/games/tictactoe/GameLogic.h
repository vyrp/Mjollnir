#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"

namespace mjollnir { namespace vigridr { 

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel();
  bool isFinished();
  int32_t getWinner();
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setHasFinished(bool value);
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setWinner(int32_t value);
  /**
   *  Specific function to use at GameLogicTest test suite.
   *  Internally should use setTableCoordinate()
   */
  void setTableCoordinate(const Coordinate& coordinate, Marker marker);
 private:
  bool checkLines_(const WorldModel& wm, Marker player);
  bool checkColumns_(const WorldModel& wm, Marker player);
  bool checkDiagonals_(const WorldModel& wm, Marker player);
  bool checkVictory_(const WorldModel& wm, Marker player, int32_t playerId);
  bool checkDraw_(const WorldModel& wm);
  void setTableCoordinate_(const Coordinate& coordinate, Marker marker);
  bool checkTableCoordinate_(const Coordinate& coordinate, Marker marker);
  bool randomPlay_(int32_t playerId);
  WorldModel worldModel_;
  int32_t player1_, player2_, winner_;
  bool hasFinished_;
  const size_t boardSize_ = 3;
};

}}  // namespaces 

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H