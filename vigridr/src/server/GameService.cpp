#include "GameService.h"

namespace mjollnir { namespace vigridr {

void GameService::gameInfo(GameInfo& gameInfo) {
  gameInfo.gameStatus = GameStatus::RUNNING;
  gameInfo.worldModel =  gameLogic_.getWorldModel();
  gameInfo.waitingTimeMiliseconds=10;
}

CommandStatus GameService::update(const Command& command) {
  gameLogic_.update(command);
  return CommandStatus::SUCCESS;
}

}}  // namespaces
