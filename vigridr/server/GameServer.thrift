/**
 * Copyright 2014 ITA
 * @author Luiz Filipe Martins Ramos (luizmramos@gmail.com)
 */
 
include "GameModel.thrift"

namespace cpp mjollnir.vigridr

service Game {
   GameModel.GameInfo gameInfo(),
}

