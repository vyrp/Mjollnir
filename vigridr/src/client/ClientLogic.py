from WorldModel.ttypes import Marker
from Command.ttypes import Command
from Command.ttypes import Coordinate
from GameModel.ttypes import GameStatus

from random import randint

def play_turn(wm):
    command = Command(Coordinate())
    while True:
        x = randint(0,2)
        y = randint(0,2)
        if wm.table[x][y] == Marker.UNMARKED:
            command.coordinate.x = x
            command.coordinate.y = y
            break
    return command


def init():
    pass