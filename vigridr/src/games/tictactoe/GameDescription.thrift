include "WorldModel.thrift"

namespace cpp mjollnir.vigridr

struct GameDescription {
  1: required WorldModel.Marker myType
}