from Command.ttypes import Action
from Command.ttypes import Command
from GameModel.ttypes import GameStatus

from random import randint

class Solution:
    def __init__(self, gameInit):
        """
        Constructor: called at the beginning of the game.
        You may do initialization here.

        Parameter:
            gameInit - not used for Wumpus
        """
        print "Python Client"

    def play_turn(self, wm, turn):
        """
        This method is called once for every turn.
        This specific example solution returns a random action.

        Parameters:
            wm -   an instance of the WorldModel class that contains an attribute called sensors of class Sensors.
                   Sensors contains the boolean attributes: breeze, stench, glitter, bump and scream.
            turn - an integer, the index of the turn.
                   If you receive twice the same number, then it means that you still have some time to think and send another command.
        Returns:
            A Command instance - a Command contains an attribute called action of type int.
                                 action must be one of the Action attributes: FORWARD, TURNRIGHT, TURNLEFT, STAY, SHOOT, GRAB and CLIMB.
        """
        command = Command()
        move = randint(0, 2)

        print str(turn) + ":",
        if move == 0:
            print "FORWARD"
            command.action = Action.FORWARD
        elif move == 1:
            print "TURNRIGHT"
            command.action = Action.TURNRIGHT
        else:
            print "TURNLEFT"
            command.action = Action.TURNLEFT

        return command

