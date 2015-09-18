include "WorldModel.thrift"

namespace cpp mjollnir.vigridr

enum PlayerType {
  PLAYER,
  COMPUTER
}

struct GameDescription {
  1: required PlayerType playerType
}

