#ifndef VIGRIDR_CLIENT_CLIENT_LOGIC_H
#define VIGRIDR_CLIENT_CLIENT_LOGIC_H

#include "../thrifts/gen-cpp/Game.h"

mjollnir::vigridr::Command playTurn(
  const mjollnir::vigridr::WorldModel& wm);

void init();

#endif  // VIGRIDR_CLIENT_CLIENT_LOGIC_H
