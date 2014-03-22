/**
 * Copyright 2014 ITA
 * @author Luiz Filipe Martins Ramos (luizmramos@gmail.com)
 */
 
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
	1: GameStatus gameStatus,
	2: i32 waitingTimeMiliseconds,
	3: optional CommandStatus commandStatus,
	4: optional string description
}
