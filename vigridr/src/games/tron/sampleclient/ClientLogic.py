from Command.ttypes import Command
from Command.ttypes import Direction
from WorldModel.ttypes import Coordinate
from GameModel.ttypes import GameStatus
from random import randint

table = [[]]
me = -1
width = 0
height = 0
dx = [1, 0 , -1, 0]
dy = [0, 1, 0, -1]
dirs = [Direction.RIGHT, Direction.UP, Direction.LEFT, Direction.DOWN]

def isValid(x,y):
  return x<width and y<height and x>=0 and y>=0

class Solution:
  def play_turn(self, wm):
    global table
    print len(table)
    table[wm.players[0].body[-1].x][wm.players[0].body[-1].y]=0
    table[wm.players[1].body[-1].x][wm.players[1].body[-1].y]=1
    for i in range(width):
      for j in range(height):
        if table[i][j]==me:
          print 'M',
        elif table[i][j]==1-me:
          print 'O',
        else:
          print '.',
      print
    print
    command = Command()
    command.direction = Direction.DOWN
    indexes = [0,1,2,3]
    for i in range(4):
      j=randint(i,3)
      aux = indexes[i]
      indexes[i] = indexes[j]
      indexes[j] = aux
    for i in indexes:
      newx = wm.players[me].body[-1].x+dx[i]
      newy = wm.players[me].body[-1].y+dy[i]
      if isValid(newx, newy) and table[newx][newy]==-1:
        command.direction = dirs[i]
        break
    return command

  def __init__(self, gameInit):
    global me
    global table
    global width
    global height
    width = gameInit.gameDescription.field.width
    height = gameInit.gameDescription.field.height
    table = [[-1 for j in range(height)] for i in range(width)]
    for i,player in enumerate(gameInit.gameInfo.worldModel.players):
      for pos in player.body:
        table[pos.x][pos.y] = i
    me = gameInit.gameDescription.myIndex
