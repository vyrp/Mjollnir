#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include <vector>
#include <utility>
#include <string>

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"
#include "../thrifts/gen-cpp/GameResult_types.h"

namespace mjollnir { namespace vigridr {

struct TotalWorldModel {};

class GameLogic {
 public:
  bool shouldPrintWorldModel(int32_t playerId);
  bool shouldIncrementCycle(int32_t playerId);
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel();
  bool isFinished();
  std::string getWinner();
  GameDescription getGameDescription(int32_t playerId) const;
  TotalWorldModel getTotalWorldModel() const;
  size_t getNumberOfPlayers() const;
  GameResult createGameResult(std::string result, int32_t id);

 private:
  bool isValidCoordinate(Coordinate pos);
  Coordinate getUpdatedHeadPosition(Coordinate pos, Direction dir);

  WorldModel worldModel_;
  TotalWorldModel twm_;
  int32_t player1_, player2_;
  std::string winner_;
  static constexpr int32_t kNoWinner = -1;
  bool hasFinished_;
  const size_t numberOfPlayers_ = 2;

  // Size of the field
  static constexpr int32_t kWidth = 20;
  static constexpr int32_t kHeight = 20;

  // Initial conditions
  const std::vector<std::pair<int, int>> kpInit[2] =
    {{{1,1}, {2,1}, {3,1}},
     {{kWidth-2,kWidth-2}, {kWidth-3,kWidth-2}, {kWidth-4,kWidth-2}}};

  // Auxiliar table
  int32_t field_[kWidth][kHeight];
  static constexpr int32_t kEmpty = -1;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H
