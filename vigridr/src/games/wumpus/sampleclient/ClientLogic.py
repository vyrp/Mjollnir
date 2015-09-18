from Command.ttypes import Action
from Command.ttypes import Command
from GameModel.ttypes import GameStatus

from random import randint

class Solution:
    def play_turn(self, wm):

        command = Command(Action())
        move = randint(0, 3)
        
        if move == 0:
            print "FORWARD"
            command.action = Action.FORWARD
        elif move == 1:
            print "TURNRIGHT"
            command.action = Action.TURNRIGHT
        elif move == 2:
            print "TURNLEFT"
            command.action = Action.TURNLEFT
        else:
            print "STAY"
            command.action = Action.STAY

        return command

    def __init__(self, gameInit):
        print gameInit.gameDescription.playerType
