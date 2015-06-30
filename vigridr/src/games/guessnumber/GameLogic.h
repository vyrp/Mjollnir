#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"

namespace mjollnir { namespace vigridr {

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel() const;
  bool isFinished() const;
  int32_t getWinner() const;
  GameDescription getGameDescription(int32_t playerId) const;
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setHasFinished(bool value);
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setWinner(int32_t value);
  /**
   *  Specific function to use at GameLogicTest test suite.
   *  Internally should use setTableCoordinate()
   */
 private:
  bool checkVictory_(Command command, int32_t playerId);
  bool randomPlay_(int32_t playerId);
  WorldModel worldModel_;
  int32_t player1_, player2_, winner_;
  bool hasFinished_;
  const size_t minValue_ = 0;
  const size_t maxValue_ = 10;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H