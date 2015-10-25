#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include <string>

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
  bool shouldPrintWorldModel(int32_t playerId);
  bool shouldIncrementCycle(int32_t playerId);
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel() const;
  size_t getWorldSize() const;
  bool isFinished() const;
  std::string getWinner() const;
  GameDescription getGameDescription(int32_t playerId) const;
  TotalWorldModel getTotalWorldModel() const;
  void initializeWorld(std::vector<std::vector<WorldSquare>>);
  size_t getNumberOfPlayers() const;
  void setWumpusPosition(int32_t, int32_t);
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setHasFinished(bool value);
  /**
   *  Specific function to use at GameLogicTest test suite
   */
  void setWinner(std::string value);
  /**
   *  Specific function to use at GameLogicTest test suite.
   *  Internally should use setTableCoordinate()
   */
 private:
  void initializeWorld_(void);
  void initializeWorldFromRusselBook_(void);
  bool checkBump_(int32_t x, int32_t y);
  void resetSensors_(void);
  void updateWorldSquare_(int32_t x, int32_t y);
  void updateSensors_(WorldSquare& ws, int32_t x, int32_t y);
  void updateWorldModel_(Action action);
  bool randomPlay_(int32_t playerId);
  
  WorldModel worldModel_;
  TotalWorldModel twm_;
  int32_t player1_, player2_, score_;
  std::string winner_;
  bool hasFinished_, wumpusAlive_, canShoot_, hasGold_;
  const size_t worldSize_ = 4;
  const size_t numberOfPlayers_ = 1;
  Direction facing_ = RIGHT;
  Coordinate playerPosition_, wumpusPosition_;
};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H

