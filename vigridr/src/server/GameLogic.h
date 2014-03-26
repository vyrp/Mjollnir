#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../server/gen-cpp/Command_types.h"
#include "../server/gen-cpp/WorldModel_types.h"

namespace mjollnir { namespace vigridr { 

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  void update(Command command, int32_t playerId);
  WorldModel getWorldModel();
};

}}  // namespaces 

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H