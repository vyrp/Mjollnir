namespace cpp mjollnir.vigridr

struct Coordinate {
  1: required i32 x,
  2: required i32 y
}

struct Player {
  1: required list<Coordinate> body
}

struct WorldModel {
  1: required list<Player> players,
}