#ifndef VIGRIDR_SERVER_GAME_SERVICE_H
#define VIGRIDR_SERVER_GAME_SERVICE_H

#include <memory>

#include "GameManager.h"
#include "../thrifts/gen-cpp/Game.h"

namespace mjollnir { namespace vigridr {

/**
 *  Service that handles the clients requests. Each client communicates
 *  with the server through a different port. So each client talks with his 
 *  own instance of the game service.
 */
class GameService : virtual public GameIf {
 public:
  GameService(std::shared_ptr<GameManager> gameManager, int32_t playerId);
  /**
   *  This method should be called when the player is ready to start the match.
   */
  void ready(GameInfo& gameInfo) override;

  /**
   *  Returns information about the current game state
   */
  void getGameInfo(GameInfo& gameInfo) override;

  /**
   *  Updates the world model based on the players command, once per turn.
   */
  CommandStatus sendCommand(const Command& command) override;

 private:
  std::shared_ptr<GameManager> gameManager_;
  int32_t playerId_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_SERVICE_H