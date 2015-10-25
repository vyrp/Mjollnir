namespace cpp mjollnir.vigridr

typedef list<i32> Point

struct WorldModel {
  1: required Point bar
  2: required list<Point> board
  3: required Point borne_off
  4: required list<i32> dice
}

