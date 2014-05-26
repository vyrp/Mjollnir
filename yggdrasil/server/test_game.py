import pdb

from game import Game

class Logger():
    def error(self, msg):
        print msg
    def info(self, msg):
        print msg

with Game('ttt2.cpp', 'ttt1.cs', 'tictactoe', Logger()) as game:
    #pdb.set_trace()
    game.download()
    game.compile()
    game.run()
    game.upload()
