#include "GameService.h"

namespace mjollnir { namespace vigridr {

GameService::GameService(std::shared_ptr<GameManager> gameManager, int32_t playerId) 
  : gameManager_(std::move(gameManager)),
    playerId_(playerId) {
}

void GameService::ready(GameInfo& gameInfo) {
  std::unique_lock<std::mutex> lock(requestingGameInfo, std::defer_lock);
  if(!lock.try_lock()) return;
  gameManager_->getGameInfo(gameInfo, playerId_);
}

void GameService::gameInfo(GameInfo& gameInfo) {
  std::unique_lock<std::mutex> lock(requestingGameInfo, std::defer_lock);
  if(!lock.try_lock()) return;
  gameManager_->getGameInfo(gameInfo, playerId_);
}

CommandStatus GameService::update(const Command& command) {
  std::unique_lock<std::mutex> lock(updating, std::defer_lock);
  if(!lock.try_lock()) return CommandStatus::ERROR;
  return gameManager_->update(command, playerId_);
}

}}  // namespaces
