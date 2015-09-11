from WorldModel.ttypes import Marker
from Command.ttypes import Command
from Command.ttypes import Coordinate
from GameModel.ttypes import GameStatus

from random import randint
import time

class Solution:
    def __init__(self, gameInit):
        print "Python Client"
        print "PlayerType: " + Marker._VALUES_TO_NAMES[gameInit.gameDescription.myType]

    def play_turn(self, wm):
        command = Command(Coordinate())
        while True:
            x = randint(0,2)
            y = randint(0,2)
            if wm.table[x][y] == Marker.UNMARKED:
                command.coordinate.x = x
                command.coordinate.y = y
                break
        print repr(command.coordinate)

        return command

