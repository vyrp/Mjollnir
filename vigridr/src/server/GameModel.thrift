include "WorldModel.thrift"

namespace cpp mjollnir.vigridr

enum GameStatus {
  RUNNING = 1,
  WAITING = 2
}

enum CommandStatus {
  SUCCESS = 1,
  ERROR = 2
}

struct GameInfo {
	1: required GameStatus gameStatus,
  2: required WorldModel.WorldModel worldModel,
	3: required i32 waitingTimeMiliseconds,
	4: optional string description  // for debugging purposes
}
