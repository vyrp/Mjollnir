from Command.ttypes import Command
from GameModel.ttypes import GameStatus
from WorldModel.ttypes import WorldModel

class Solution:
    def __init__(self, gameInit):
        """
        Constructor: called at the beginning of the game.
        You may do initialization here.

        Parameter:
            gameInit - depends on the game. It will contain necessary information for initialization.
        """
        pass

    def play_turn(self, wm, turn):
        """
        This method is called once for every turn.
        This specific example solution returns an empty action.

        Parameters:
            wm -   depends on the game. It will contain the observable part of the world model.
            turn - an integer, the index of the turn.
                   If you receive twice the same number, don't worry, just ignore it.
        Returns:
            A Command instance - depends on the game. It's your command for this turn.
        """
        return None

    def end_of_game(self, result)
        """
        This method is called at the end of the game.

        Parameters:
            result - depends on the game. It will contain the result of the game.
        """
        pass
