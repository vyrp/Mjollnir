#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include <vector>
#include <utility>

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"

namespace mjollnir { namespace vigridr {

/**
 *  This class should be implemented for each game providing the
 *  logic necessary to run the game
 */
class GameLogic {
 public:
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
   *  Tells weather the game has finished or not
   */
  bool isFinished() const;

  /**
   *  Returns the winner (only called if isFinished()==true)
   *  In case of a tie returns -1
   */
  int32_t getWinner() const;

  /**
   *  Returns the gameDescription.
   *  Called when the player connects for the first time (and never again)
   *  Should provide initialization info
   */
  GameDescription getGameDescription(int32_t playerId) const;


 private:
  WorldModel worldModel_;
  int32_t player1_;
  int32_t player2_;
  int32_t winner_;
  static const int32_t kNoWinner = -1;
  bool hasFinished_;

};

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H
