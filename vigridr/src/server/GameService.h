#ifndef VIGRIDR_SERVER_GAME_SERVICE_H
#define VIGRIDR_SERVER_GAME_SERVICE_H

#include "GameLogic.h"
#include "gen-cpp/Game.h"

namespace mjollnir { namespace vigridr {

class GameService : virtual public GameIf {
 public:
  void gameInfo(GameInfo& gameInfo) override;
  CommandStatus update(const Command& command) override;
 private:
  GameLogic gameLogic_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_SERVICE_H