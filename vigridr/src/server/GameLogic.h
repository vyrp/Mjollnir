#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include <vector>
#include <utility>

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"

namespace mjollnir { namespace vigridr { 

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel();
  bool isFinished();
  int32_t getWinner();

 private:
  bool isValidCoordinate(Coordinate pos);
  Coordinate getUpdatedHeadPosition(Coordinate pos, Direction dir);

  WorldModel worldModel_;
  int32_t player1_, player2_, winner_;
  static constexpr int32_t kNoWinner = -1;
  bool hasFinished_;

  // Size of the field
  static constexpr int32_t kWidth = 25;
  static constexpr int32_t kHeight = 25;

  // Initial conditions
  const std::vector<std::pair<int, int>> kpInit[2] = 
    {{{1,1}, {2,1}, {3,1}},
     {{23,23}, {22,23}, {21,23}}};

  // Auxiliar table
  int32_t field_[kWidth][kHeight];
  static constexpr int32_t kEmpty = -1;
};

}}  // namespaces 

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H