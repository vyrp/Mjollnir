#ifndef VIGRIDR_SERVER_GAME_LOGIC_H
#define VIGRIDR_SERVER_GAME_LOGIC_H

#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/Command_constants.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameDescription_types.h"
#include "../thrifts/gen-cpp/GameResult_types.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace mjollnir { namespace vigridr {

struct TotalWorldModel {};

Move make_move(int32_t src, int32_t dst);

std::ostream& operator<<(std::ostream& os, Move m);

std::ostream& operator<<(std::ostream& os, Command c);

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> v);

class GameLogic {
 public:
  bool shouldPrintWorldModel(int32_t playerId);
  bool shouldIncrementCycle(int32_t playerId);
  GameLogic(int32_t playerId1, int32_t playerId2);
  bool update(Command command, int32_t playerId);
  WorldModel getWorldModel() const;
  bool isFinished() const;
  std::string getWinner() const;
  GameDescription getGameDescription(int32_t playerId) const;
  TotalWorldModel getTotalWorldModel() const;
  size_t getNumberOfPlayers() const;
  GameResult createGameResult(std::string result, int32_t id);

  // Number of checkers for each player. Short name because it is used a lot.
  const static int32_t NC = 15;

  // Number of points on the board. Short name because it is used a lot.
  const static size_t NP = 24;

  const int32_t BEAR_OFF = g_Command_constants.BEAR_OFF;
  const int32_t FROM_BAR = g_Command_constants.FROM_BAR;

 protected:
  bool all_checkers_in_home_board_(WorldModel wm, PlayerColor color);
  std::vector<Command> calculate_possibilities_(WorldModel wm, Command command, PlayerColor color);
  PlayerColor color_(int32_t playerId) const;
  std::vector<Command> filter_commands_(const std::vector<Command>& possible_commands, PlayerColor color);
  void move_(int32_t src, int32_t dst, PlayerColor color);
  void rollDice_();
  bool try_move_(const WorldModel& wm, int32_t src, int32_t dst, PlayerColor color, WorldModel& new_wm);

  void setBoard_forTest(const std::vector<Point>& board);
  void setDice_forTest(const std::vector<int32_t>& dice);

  WorldModel worldModel_;
  TotalWorldModel twm_;
  int32_t player1_, player2_;
  std::string winner_;
  std::unordered_map<int32_t, bool> playerIdToInvalid_;
  bool hasFinished_;
  const size_t numberOfPlayers_ = 2;
};

Point make_point(int32_t reds, int32_t whites);

}}  // namespaces

#endif  // VIGRIDR_SERVER_GAME_LOGIC_H
