from WorldModel.ttypes import Marker
from Command.ttypes import Command
from Command.ttypes import Coordinate
from GameModel.ttypes import GameStatus

from random import randint

class Solution:
    def play_turn(self, wm):
        for i in range(3):
            for j in range(3):
                if wm.table[i][j] == Marker.X:
                    print 'X',
                elif wm.table[i][j] == Marker.O:
                    print 'O',
                else:
                    print '.',
            print
        print

        command = Command(Coordinate())
        while True:
            x = randint(0,2)
            y = randint(0,2)
            if wm.table[x][y] == Marker.UNMARKED:
                command.coordinate.x = x
                command.coordinate.y = y
                break
        return command

    def __init__(self, gameInit):
        if gameInit.gameDescription.myType == Marker.X:
            print "PlayerType: X"
        else:
            print "PlayerType: O"