namespace cpp mjollnir.vigridr

enum Direction {
  UP = 1,
  DOWN = 2,
  LEFT = 3,
  RIGHT = 4
}

struct Command {
  1: required Direction direction
}