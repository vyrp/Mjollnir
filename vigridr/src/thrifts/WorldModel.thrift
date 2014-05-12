namespace cpp mjollnir.vigridr

struct Coordinate {
  1: required i32 x,
  2: required i32 y
}

struct Player {
  1: required list<Coordinate> body
}

struct Field {
  1: i32 width,
  2: i32 height
}

struct WorldModel {
  1: optional Field field,
  2: required list<Player> players,
}