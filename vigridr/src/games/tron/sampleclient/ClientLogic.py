from Command.ttypes import Command
from Command.ttypes import Direction
from WorldModel.ttypes import Coordinate
from GameModel.ttypes import GameStatus
from random import randint, shuffle

dx = [1, 0 , -1, 0]
dy = [0, 1, 0, -1]
dirs = [Direction.RIGHT, Direction.UP, Direction.LEFT, Direction.DOWN]

class Solution:
    def __init__(self, gameInit):
        """
        Constructor: called at the beginning of the game.
        You may do initialization here.

        Parameter:
            gameInit - contains a field named gameDescription, which itself contains fields myIndex and 'field'.
                       myIndex is an int and represents the index of the world model table of your snake.
                       'field' is an instance of the class Field, and contains the fields width and height, of type int.
                       It also contains a field called gameInfo with another field, worldModel, of type WorldModel, described below.
        """

        print "Python Client"

        self.width = gameInit.gameDescription.field.width
        self.height = gameInit.gameDescription.field.height
        self.table = [[-1 for j in range(self.height)] for i in range(self.width)]

        for i, player in enumerate(gameInit.gameInfo.worldModel.players):
            for pos in player.body:
                self.table[pos.x][pos.y] = i

        self.me = gameInit.gameDescription.myIndex

    def play_turn(self, wm, turn):
        """
        This method is called once for every turn.
        This specific example solution stores a matrix with the snakes bodies (as 0 or 1, with -1 as empty cell)
        and returns a random valid direction.

        Parameters:
            wm   - an instance of the WorldModel class that contains a field called players which is a list of Players.
                   A Player has a field named body which is a list of Coordinates,
                   and which represent the coordinates of the body parts of the snake, in order.
                   A Coordinate has two fields, x and y, of type int.
            turn - an integer, the index of the turn.
                   If you receive twice the same number, don't worry, just ignore it.

        Returns:
            A Command instance - a Command contains a field called direction of type int,
                                 which can be one of the Direction values: RIGHT, UP, LEFT, DOWN.
        """
        print len(self.table)

        # Place new heads
        self.table[wm.players[0].body[-1].x][wm.players[0].body[-1].y] = 0
        self.table[wm.players[1].body[-1].x][wm.players[1].body[-1].y] = 1

        # Print the board
        print "Turn", turn
        for i in range(self.width):
            for j in range(self.height):
                if self.table[i][j] == self.me:
                    print 'M',
                elif self.table[i][j] == 1 - self.me:
                    print 'O',
                else:
                    print '.',
            print
        print

        command = Command()
        command.direction = Direction.DOWN

        # Randomly choose a direction
        indexes = range(4)
        shuffle(indexes)
        for i in indexes:
            newx = wm.players[self.me].body[-1].x + dx[i]
            newy = wm.players[self.me].body[-1].y + dy[i]
            if self.isValid(newx, newy) and self.table[newx][newy] == -1:
                command.direction = dirs[i]
                break

        return command

    def isValid(self, x, y):
        return x < self.width and y < self.height and x >= 0 and y >= 0

