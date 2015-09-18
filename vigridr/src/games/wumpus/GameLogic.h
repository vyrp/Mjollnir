#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"

namespace mjollnir { namespace vigridr {

enum Direction{
  UP,
  RIGHT,
  DOWN,
  LEFT
};

struct Coordinate{
  int32_t x, y;
};

struct WorldSquare{
  bool gold = false;
  bool wumpus = false;
  bool player = false;
  bool pit = false;
  bool breeze = false;
  bool stench = false;
};

struct TotalWorldModel{
  std::vector<std::vector<WorldSquare>> map;
  Direction playerDirection;
};

class GameLogic {
 public:
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel() const;
  bool isFinished() const;
  int32_t getWinner() const;
  GameDescription getGameDescription(int32_t playerId) const;
  TotalWorldModel getTotalWorldModel() const;
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
  void initializeWorld_(void);
  bool checkBump_(int32_t x, int32_t y);
  void resetSensors_(void);
  void updateWorldSquare_(int32_t x, int32_t y);
  void updateSensors_(WorldSquare& ws, int32_t x, int32_t y);
  void updateWorldModel_(Action action);
  bool randomPlay_(int32_t playerId);
  
  WorldModel worldModel_;
  TotalWorldModel twm_;
  int32_t player1_, player2_, winner_, score_;
  bool hasFinished_, wumpusAlive_, canShoot_, hasGold_;
  const size_t worldSize_ = 4;
  Direction facing_ = RIGHT;
  Coordinate playerPosition_, wumpusPosition_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H