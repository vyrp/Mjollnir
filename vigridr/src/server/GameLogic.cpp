#include "GameLogic.h"

#include <stdio.h>

namespace mjollnir { namespace vigridr {

void GameLogic::update(Command command) {
  printf("Updating...\n");
  return;
}

WorldModel GameLogic::getWorldModel() {
  printf("worldModel\n");
  return WorldModel();
}

}}