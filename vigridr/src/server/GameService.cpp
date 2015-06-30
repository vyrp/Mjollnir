#include "GameService.h"

namespace mjollnir { namespace vigridr {

GameService::GameService(std::shared_ptr<GameManager> gameManager, int32_t playerId)
  : gameManager_(std::move(gameManager)),
    playerId_(playerId) {
}

void GameService::ready(GameInit& gameInit) {
  gameManager_->getGameInit(gameInit, playerId_);
}

void GameService::getGameInfo(GameInfo& gameInfo) {
  gameManager_->getGameInfo(gameInfo, playerId_);
}

CommandStatus GameService::sendCommand(const Command& command) {
  return gameManager_->update(command, playerId_);
}

}}  // namespaces
