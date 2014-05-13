include "GameModel.thrift"
include "Command.thrift"

namespace cpp mjollnir.vigridr

service Game {
  GameModel.GameInit ready(),
  GameModel.GameInfo getGameInfo(),
  GameModel.CommandStatus sendCommand(1:Command.Command command)
}

