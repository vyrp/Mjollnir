import os
import sys
import shutil as sh
import glob
import change_game_code as cc


def cache_state(game):
  if game not in os.listdir(cc.games_dir):
    print 'The game "' + game + '" is not in games folder'
    return 1

  game_dir = os.path.join(cc.games_dir, game)

  gen_files = glob.glob(os.path.join(cc.thrifts_dir, "gen-*"))
  obj_files = [os.path.join(cc.obj_dir, "utils")]
  obj_files.append(os.path.join(cc.obj_dir, "thrifts"))
  obj_files.append(os.path.join(cc.obj_dir, "server"))
  bin_file = os.path.join(cc.bin_dir, "cpp/server")

  cc.copy_force(gen_files, game_dir)

  obj_dir = os.path.join(game_dir, "obj")
  try:
    sh.rmtree(obj_dir)
  except:
    pass

  os.makedirs(obj_dir, 0777);
  cc.copy_force(obj_files, obj_dir)

  bin_dir = os.path.join(game_dir, "bin")
  try:
    sh.rmtree(bin_dir)
  except:
    pass
  os.makedirs(bin_dir, 0777);
  cc.copy([bin_file], bin_dir)

if __name__=="__main__":
  cache_state(sys.argv[1])