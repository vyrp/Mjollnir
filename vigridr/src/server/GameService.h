#ifndef VIGRIDR_SERVER_GAME_SERVICE_H
#define VIGRIDR_SERVER_GAME_SERVICE_H

#include <memory>

#include "GameLogic.h"
#include "gen-cpp/Game.h"

namespace mjollnir { namespace vigridr {

class GameService : virtual public GameIf {
 public:
  GameService(std::shared_ptr<GameLogic> gameLogic, int32_t playerId);
  void gameInfo(GameInfo& gameInfo) override;
  CommandStatus update(const Command& command) override;
 private:
  std::shared_ptr<GameLogic> gameLogic_;
  int32_t playerId_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_SERVICE_H