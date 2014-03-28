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
 private:
  bool checkLines(WorldModel wm, Marker player);
  bool checkColumns(WorldModel wm, Marker player);
  bool checkDiagonals(WorldModel wm, Marker player);
  WorldModel worldModel;
  int32_t player1, player2;
};

}}  // namespaces 

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H