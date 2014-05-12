namespace cpp mjollnir.vigridr

enum Marker {
  UNMARKED = 1,
  O = 2,
  X = 3
}

struct WorldModel {
  1: list<list<Marker>> table;
}