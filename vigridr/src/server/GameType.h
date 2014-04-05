#ifndef VIGRIDR_SERVER_GAME_TYPE
#define VIGRIDR_SERVER_GAME_TYPE

namespace mjollnir { namespace vigridr {

enum class GameType {
  TURN,  // e.g. Chess
  CONTINUOUS  // e.g. Tron
};

}}

#endif  // VIGRIDR_SERVER_GAME_TYPE