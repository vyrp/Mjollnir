from Command.ttypes import Command
from GameModel.ttypes import GameStatus

from random import randint

class Solution:
    def play_turn(self, wm):
        command = Command(randint(1, 10))
        #print command.number
        return command

    def __init__(self, gameInit):
        pass
