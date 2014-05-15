#include "GameManager.h"

#include <stdexcept>

#include "../utils/Log.h"
#include "../utils/MapUtils.h"
#include "GameType.h"
#include "GameConfig.h"

namespace mjollnir { namespace vigridr {

void PlayerTurnData::init(int32_t id) {
  setId(id);
  clearCommand();
}

void PlayerTurnData::setCommand(Command command, time_point time) {
  isCommandSet_ = true;
  lastUpdateTime_ = time;
  command_ = std::move(command);
}

void PlayerTurnData::clearCommand() {
  isCommandSet_ = false;
}

void PlayerTurnData::setIsTurn(bool isTurn) {
  isTurn_ = isTurn;
}

GameManager::GameManager(int32_t playerId0, int32_t playerId1) 
  : idToIdx_({{playerId0, 0}, {playerId1, 1}}),
    idxToId_({{playerId0, playerId1}}),
    gameLogic_(playerId0, playerId1) {
  initializeGame(playerId0, playerId1);
  updaterThread_ = std::thread([this]() { 
    updaterTask(); 
  });
}

void GameManager::finalizeGame(bool success) {
  gameInfo_.gameStatus = GameStatus::FINISHED;
  gameInfo_.worldModel = gameLogic_.getWorldModel();
}

void GameManager::initializeGame(int32_t playerId0, int32_t playerId1) {
  std::unique_lock<std::mutex> lock0(playerMutex_[0], std::defer_lock);
  std::unique_lock<std::mutex> lock1(playerMutex_[1], std::defer_lock);
  std::unique_lock<std::mutex> lock2(gameInfoMutex_, std::defer_lock);
  std::lock(lock0, lock1, lock2);
  gameInfo_.worldModel = gameLogic_.getWorldModel();
  playerTurnData_[0].init(playerId0);
  playerTurnData_[1].init(playerId1);
  playerTurnData_[0].setIsTurn(true);
  playerTurnData_[1].setIsTurn(true);
  if (config::gameType == GameType::TURN) {
    playerTurnData_[1].setIsTurn(false);
  }
  gameInfo_.cycle = 0;
  gameInfo_.gameStatus = GameStatus::WAITING;
  timer_.startFirstCycle();
}

void GameManager::nextTurn() {
  timer_.sleepUntilWorldModelUpdateTime();
  std::unique_lock<std::mutex> lock0(playerMutex_[0], std::defer_lock);
  std::unique_lock<std::mutex> lock1(playerMutex_[1], std::defer_lock);
  std::unique_lock<std::mutex> lock2(gameInfoMutex_, std::defer_lock);
  std::lock(lock0, lock1, lock2);
  if (config::gameType == GameType::TURN) {
    for (size_t i = 0; i < kMaxPlayers; ++i) {
        playerTurnData_[i].setIsTurn(!playerTurnData_[i].isTurn());   
    } 
  }
  gameInfo_.cycle++;
  gameInfo_.gameStatus = GameStatus::RUNNING;
  gameInfo_.worldModel = gameLogic_.getWorldModel();
  timer_.sleepUntilWorldModelTime();
  timer_.startCycle();
}

bool 
checkCommands(const std::array<PlayerTurnData, kMaxPlayers>& playerTurnData) {
  for (size_t i = 0; i < kMaxPlayers; ++i) {
    if (playerTurnData[i].isTurn() && !playerTurnData[i].isCommandSet()) {
      std::cout << "[ERROR]: No command set for player " << i << std::endl;
      return false;
    } 
  }
  return true;
}

void 
clearCommands(std::array<PlayerTurnData, kMaxPlayers>& playerTurnData) {
  if(playerTurnData[0].isTurn()) { playerTurnData[0].clearCommand(); }
  if(playerTurnData[1].isTurn()) { playerTurnData[1].clearCommand(); }
}

// updater task is a task that is executed to update the game every cycle
void GameManager::updaterTask() {
  bool errorHappened = true;
  while (true) {
    nextTurn();  // initialize next turn
    timer_.sleepUntilPlayerUpdateTime();
    std::cout << "Updating..." << std::endl;
    std::array<PlayerTurnData, kMaxPlayers> movements;
    {
      std::unique_lock<std::mutex> lock0(playerMutex_[0], std::defer_lock);
      std::unique_lock<std::mutex> lock1(playerMutex_[1], std::defer_lock);
      // locks player 1 and 2 simultaneously so that there is no dead lock
      std::lock(lock0, lock1); 
      // check wheather the players sent the command
      errorHappened = !checkCommands(playerTurnData_);
      if (errorHappened) break;
      movements = playerTurnData_;  // copy assignment operation
      // now we can release the locks so that other threads can change
      // playerTurnData without compromising the game update task
      // btw playerTurnData is the data sent from a player in a given turn
      clearCommands(playerTurnData_);
    }
    // sorting the player movements by update time (who sent first moves first)
    std::sort(movements.begin(), movements.end(), 
      [](const PlayerTurnData& a, const PlayerTurnData& b) {
        return a.getLastUpdatedTime() < b.getLastUpdatedTime();
    });

    // only execute movements if it is the player's turn
    int32_t countWrongPlayers = 0;  // how many players sent invalid command
    int32_t correctPlayer = 0;  // if one is wrong this variable  
    for (auto& playerMove : movements) {
      if (!playerMove.isTurn()) { 
        correctPlayer = playerMove.getId();  // if it is not my turn i'm valid
        continue; 
      }
      bool validCommand = 
        gameLogic_.update(playerMove.getCommand(), playerMove.getId());
      if (!validCommand) {
        countWrongPlayers++;
      } else {
        correctPlayer = playerMove.getId();  // valid command
      }
    }
    bool finished = gameLogic_.isFinished();
    int32_t winner = gameLogic_.getWinner();

    // if one player sent an invalid command the other one wins
    if(countWrongPlayers == 1) {
      winner = correctPlayer;
      finished = true;
    } else if (countWrongPlayers == 2) {
      winner = -1;
      finished = 1;
    }
    if (finished) {
      std::cout << "Winner is " << winner << std::endl;
      break;
    }
  }
  // game ends
  finalizeGame(!errorHappened);
}

CommandStatus GameManager::update(const Command& command, int32_t playerId) {
  int32_t idx = utils::getDefault(idToIdx_, playerId, -1);
  CHECK((idx == 0 || idx == 1), "Unknown player with id %d", idx);
  {
    std::unique_lock<std::mutex> lock(playerMutex_[idx]);
    playerTurnData_[idx].setCommand(command, clock::now());
  }
  return CommandStatus::SUCCESS;
}

void GameManager::getGameInfo(GameInfo& gameInfo, int32_t playerId) {
  {
    std::unique_lock<std::mutex> lock(gameInfoMutex_);
    gameInfo = gameInfo_;
  }
  gameInfo.updateTimeLimitMs = timer_.getPlayerUpdateTime();
  // +1 just to make sure
  gameInfo.nextWorldModelTimeEstimateMs = timer_.getWorldModelTime() + 1;
  {
    const size_t kInvalid = 10000;
    size_t idx = utils::getDefault(idToIdx_, playerId, kInvalid);
    CHECK(idx != kInvalid, "Unknown player with id %d.", idx);
    std::unique_lock<std::mutex> lock(playerMutex_[idx]);
    gameInfo.isMyTurn = playerTurnData_[idx].isTurn();
  }
  printf("%d %d\n", 
         gameInfo.updateTimeLimitMs, gameInfo.nextWorldModelTimeEstimateMs);
}  

void GameManager::getGameInit(GameInit& gameInit, int32_t playerId) {
  getGameInfo(gameInit.gameInfo, playerId);
  gameInit.gameDescription = gameLogic_.getGameDescription(playerId);
}

}}

