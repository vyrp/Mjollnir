#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include <vector>
#include <utility>
#include <string>

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"

namespace mjollnir { namespace vigridr {

  struct TotalWorldModel {};

/**
 *  This class should be implemented for each game providing the
 *  logic necessary to run the game
 */
class GameLogic {
 public:
  /**
  *  Tells GameManager if it should print the world model
  *  It should't exist if Vigridr is refactored to accept any
  *  number of players
  */
  bool shouldPrintWorldModel(int32_t playerId);
  /**
   *  Each player has an unique id
   */
  GameLogic(int32_t playerId1, int32_t playerId2);

  /**
   *  Method to update  the world model given a player command
   *  Don't worry with multi threading
   *  The player who sends first executes first
   *  Returns true if successfully updated
   */
  bool update(Command command, int32_t playerId);

  /**
   *  Returns the world model
   */
  WorldModel getWorldModel() const;

  /**
   *  Returns the total world model which is the world model for
   *  partially observable games
   */
  TotalWorldModel getTotalWorldModel() const;

  /**
   *  Tells weather the game has finished or not
   */
  bool isFinished() const;

  /**
   *  Returns the winner (only called if isFinished()==true)
   *  In case of a tie returns -1
   *  In case of a score return s:score
   */
  std::string getWinner() const;

  /**
   *  Returns the gameDescription.
   *  Called when the player connects for the first time (and never again)
   *  Should provide initialization info
   */
  GameDescription getGameDescription(int32_t playerId) const;
  /**
   * Returns the number of players for the game
   */
  size_t getNumberOfPlayers() const;  


 private:
  WorldModel worldModel_;
  TotalWorldModel twm_;
  int32_t player1_;
  int32_t player2_;
  std::string winner_;
  static const std::string kNoWinner;
  bool hasFinished_;
  const size_t numberOfPlayers_ = 2;

};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H
