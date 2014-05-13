namespace cpp mjollnir.vigridr

struct Field {
  1: i32 width,
  2: i32 height
}

struct GameDescription {
  1: required i32 myIndex,
  2: required Field field
}