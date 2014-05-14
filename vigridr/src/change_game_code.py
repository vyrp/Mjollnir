import sys
import shutil as sh
import os
import glob

games_dir = './games'
server_dir = './server'
client_dir = './client'
test_dir = './test'
thrifts_dir = './thrifts'

def copy(files, dest_dir):
  for file in files:
    if os.path.isfile(file):
      sh.copy(file, dest_dir)

def change_game_code(game, copy_sample_clients, copy_tests):
  if game not in os.listdir(games_dir):
    print 'The game "' + game + '" is not in games folder'
    return 1

  game_dir = os.path.join(games_dir, game)

  server_cpp_files = glob.glob(os.path.join(game_dir, "*.cpp"))
  server_h_files = glob.glob(os.path.join(game_dir, "*.h"))
  thrift_files = glob.glob(os.path.join(game_dir, "*.thrift"))

  copy(server_cpp_files, server_dir)
  copy(server_h_files, server_dir)
  copy(thrift_files, thrifts_dir)
  
  if copy_sample_clients:
    client_sample_files = glob.glob(os.path.join(game_dir, "sampleclient/*"))
    copy(client_sample_files, client_dir)

  if copy_tests:
    test_files = glob.glob(os.path.join(game_dir, "test/*"))
    copy(test_files, test_dir)

  return 0


if __name__ == "__main__":
  change_game_code(sys.argv[1], "--with-client" in sys.argv, "--with-test" in sys.argv)