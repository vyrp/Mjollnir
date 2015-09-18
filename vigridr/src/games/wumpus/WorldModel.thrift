namespace cpp mjollnir.vigridr

struct Sensors {
	1: required bool breeze = false,
	2: required bool stench = false,
	3: required bool glitter = false,
	4: required bool bump = false,
	5: required bool scream = false 
}

struct WorldModel {
  1: required Sensors sensors
}
