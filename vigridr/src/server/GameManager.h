#ifndef VIGRIDR_SERVER_GAME_MANAGER
#define VIGRIDR_SERVER_GAME_MANAGER

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

#include "GameLogic.h"
#include "GameTimer.h"
#include "../thrifts/gen-cpp/Command_types.h"
#include "../thrifts/gen-cpp/WorldModel_types.h"
#include "../thrifts/gen-cpp/GameModel_types.h"

namespace mjollnir { namespace vigridr {

constexpr size_t kMaxPlayers = 2;

/*
 *  Data structure with the information about the player's command in a turn
 */
class PlayerTurnData {
 public:
  void init(int32_t id);
  void setCommand(Command command, time_point time);  
  void clearCommand();
  void setIsTurn(bool isTurn);

  Command getCommand() const { return command_; }
  bool isCommandSet() const { return isCommandSet_; }
  time_point getLastUpdatedTime() const { return lastUpdateTime_; };
  bool isTurn() const { return isTurn_; }
  int32_t getId() const { return id_; }
 private:
  void setId(int32_t id) { id_ = id; }

  int32_t id_;
  Command command_;
  bool isCommandSet_;
  time_point lastUpdateTime_;
  bool isTurn_;
};

/**
 *  The GameManager gets the requests from both player, and synchronizes them.
 *  Protocol: 
 *  1- Clients say ready() and get gameInfo with first world model time in ms.
 *  2- Clients ask for gameInfo() when world model is ready.
 *  3- Clients proccess gameInfo and make decision before updateTimeLimitMs.
 *  4- Clients inform the decision via sendMessage().
 *  5- Server update world model using clients command.
 *  6- Clients get next world model after nextWorldModelTime via gameInfo()
 */
class GameManager {
 public:
  GameManager(int32_t playerId0, int32_t playerId1);
  CommandStatus update(const Command& command, int32_t playerId);
  void getGameInfo(GameInfo& gameInfo, int32_t playerId);

 private:
  void updaterTask();
  void updateTime(const std::chrono::milliseconds& d);
  void nextTurn();
  void initializeGame(int32_t playerId0, int32_t playerId1);
  void finalizeGame(bool success);

  GameInfo gameInfo_;
  std::array<PlayerTurnData, kMaxPlayers> playerTurnData_;
  std::map<int32_t, size_t> idToIdx_;  // from id to index
  std::array<int32_t, kMaxPlayers> idxToId_;  // from index to id

  std::mutex gameInfoMutex_;
  std::array<std::mutex, kMaxPlayers> playerMutex_;

  GameTimer timer_;

  GameLogic gameLogic_;
  std::thread updaterThread_;
};

}}


#endif  // VIGRIDR_SERVER_GAME_MANAGER
