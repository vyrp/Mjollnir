from game import Game

class Logger():
    def error(self, msg):
        print msg

with Game('ttt_1.cpp', 'ttt_2.cpp', 'tictactoe', Logger()) as game:
    game.download()
    game.compile()
    game.run()
    game.upload()
