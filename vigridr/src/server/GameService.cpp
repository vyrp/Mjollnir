#include "GameService.h"

namespace mjollnir { namespace vigridr {

GameService::GameService(std::shared_ptr<GameLogic> gameLogic, int32_t playerId) 
  : gameLogic_(std::move(gameLogic)),
    playerId_(playerId) {
  printf("playerId %d\n", playerId);
}

void GameService::gameInfo(GameInfo& gameInfo) {
  gameInfo.gameStatus = GameStatus::RUNNING;
  gameInfo.worldModel =  gameLogic_->getWorldModel();
  gameInfo.waitingTimeMiliseconds=10;
}

CommandStatus GameService::update(const Command& command) {
  gameLogic_->update(command, playerId_);
  return CommandStatus::SUCCESS;
}

}}  // namespaces
