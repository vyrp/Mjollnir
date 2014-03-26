#include "GameLogic.h"

#include <stdio.h>

namespace mjollnir { namespace vigridr {

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
}

void GameLogic::update(Command command, int32_t playerId) {
  printf("Updating... player %d\n", playerId);
  return;
}

WorldModel GameLogic::getWorldModel() {
  printf("worldModel\n");
  return WorldModel();
}

}}