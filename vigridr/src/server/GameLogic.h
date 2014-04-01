#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../server/gen-cpp/Command_types.h"
#include "../server/gen-cpp/WorldModel_types.h"

namespace mjollnir { namespace vigridr { 

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel();
  bool isFinished();
  int32_t getWinner();
 private:
  bool checkLines(const WorldModel& wm, Marker player);
  bool checkColumns(const WorldModel& wm, Marker player);
  bool checkDiagonals(const WorldModel& wm, Marker player);
  bool checkVictory(const WorldModel& wm, Marker player, int32_t playerId);
  void setTableCoordinate(const Coordinate& coordinate, Marker marker);
  bool checkTableCoordinate(const Coordinate& coordinate, Marker marker);
  bool randomPlay(int32_t playerId);
  WorldModel worldModel;
  int32_t player1, player2, winner;
  bool hasFinished;
  const size_t boardSize = 3;
};

}}  // namespaces 

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H