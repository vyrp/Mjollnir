include "GameModel.thrift"
include "Command.thrift"

namespace cpp mjollnir.vigridr

service Game {
  GameModel.GameInfo gameInfo(),
  GameModel.CommandStatus update(1:Command.Command command)
}

