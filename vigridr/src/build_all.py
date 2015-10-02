from subprocess import check_call as call
import os
from change_game_code import change_game_code
from cache_state import cache_state

game_list = ['template', 'tron', 'tictactoe', 'wumpus', 'guessnumber']

def build_all():
  os.chdir('..')
  call(['make', 'directories'])
  call(['make', 'remove'])
  os.chdir("src")
  for game in game_list:
    change_game_code(game, copy_sample_clients=False, copy_tests=True, copy_obj=False)
    call(['make', 'remove'])
    call(['make', 'server'])
    call(['make', 'buildtests'])
    call(['make', 'test'])
    cache_state(game)
  
  change_game_code(game_list[0], copy_sample_clients=False, copy_tests=True, copy_obj=False)

if __name__ == "__main__":
  build_all()
