#ifndef VIGRIDR_SERVER_GAME_SERVICE_H
#define VIGRIDR_SERVER_GAME_SERVICE_H

#include <memory>
#include <mutex>

#include "GameManager.h"
#include "gen-cpp/Game.h"

namespace mjollnir { namespace vigridr {

class GameService : virtual public GameIf {
 public:
  GameService(std::shared_ptr<GameManager> gameManager, int32_t playerId);
  void gameInfo(GameInfo& gameInfo) override;
  void ready(GameInfo& gameInfo) override;
  CommandStatus update(const Command& command) override;
 private:
  std::mutex updating;
  std::mutex requestingGameInfo;

  std::shared_ptr<GameManager> gameManager_;
  int32_t playerId_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_SERVICE_H