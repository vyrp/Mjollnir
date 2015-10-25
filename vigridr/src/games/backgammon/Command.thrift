namespace cpp mjollnir.vigridr

const i32 FROM_BAR = -1;
const i32 BEAR_OFF = -2;

struct Move {
  1: required i32 src,
  2: required i32 dst
}

struct Command {
  1: required list<Move> moves
}
