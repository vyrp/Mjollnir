include "GameModel.thrift"

namespace cpp mjollnir.vigridr

service Game {
   GameModel.GameInfo gameInfo(),
}

