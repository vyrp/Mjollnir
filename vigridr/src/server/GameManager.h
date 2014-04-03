#ifndef VIGRIDR_SERVER_GAME_MANAGER
#define VIGRIDR_SERVER_GAME_MANAGER

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

#include "GameLogic.h"
#include "gen-cpp/Command_types.h"
#include "gen-cpp/WorldModel_types.h"
#include "gen-cpp/GameModel_types.h"

namespace mjollnir { namespace vigridr {

constexpr size_t kMaxPlayers = 2;
typedef std::chrono::high_resolution_clock game_clock;
typedef std::chrono::time_point<game_clock> game_time;

class PlayerTurnData {
 public:
  void init(int32_t id);
  void setCommand(Command command, game_time time);  
  void resetCommand();
  void setIsTurn(bool isTurn);

  Command getCommand() const { return command_; }
  bool isCommandSet() const { return isCommandSet_; }
  game_time getLastUpdatedTime() const { return lastUpdateTime_; };
  bool isTurn() const { return isTurn_; }
  int32_t getId() const { return id_; }
 private:
  void setId(int32_t id) { id_ = id; }

  int32_t id_;
  Command command_;
  bool isCommandSet_;
  game_time lastUpdateTime_;
  bool isTurn_;
};

class GameManager {
 public:
  GameManager(int32_t playerId0, int32_t playerId1);
  CommandStatus update(const Command& command, int32_t playerId);
  void getGameInfo(GameInfo& gameInfo, int32_t playerId);

 protected:
  GameInfo gameInfo_;
  std::array<PlayerTurnData, kMaxPlayers> playerTurnData_;
  std::map<int32_t, size_t> idToIdx_;
  std::array<int32_t, kMaxPlayers> idxToId_;

  std::mutex gameInfoMutex_;
  std::array<std::mutex, kMaxPlayers> playerMutex_;

  game_time nextUpdateTime_;
  game_time nextWorldModelTime_;

  void nextTurn();
  void init();

 private:
  void execute(const PlayerTurnData& turnData);
  void updaterTask();
  void updateTime(const std::chrono::milliseconds& d);

  GameLogic gameLogic_;
  std::thread updaterThread_;
};

}}


#endif  // VIGRIDR_SERVER_GAME_MANAGER
