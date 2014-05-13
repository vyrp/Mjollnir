import sys
import shutil as sh
import os
import glob

games_dir = './games'
server_dir = './server'
client_dir = './client'
thrifts_dir = './thrifts'

def copy(files, dest_dir):
  for file in files:
    if os.path.isfile(file):
      sh.copy(file, dest_dir)

def main():
  game = sys.argv[1]
  if game not in os.listdir(games_dir):
    print 'The game "' + game + '" is not in games folder'
    exit(1)

  game_dir = os.path.join(games_dir, game)

  server_cpp_files = glob.glob(os.path.join(game_dir, "*.cpp"))
  server_h_files = glob.glob(os.path.join(game_dir, "*.h"))
  client_sample_files = glob.glob(os.path.join(game_dir, "sampleclient/*"))
  thrift_files = glob.glob(os.path.join(game_dir, "*.thrift"))

  copy(server_cpp_files, server_dir)
  copy(server_h_files, server_dir)
  copy(thrift_files, thrifts_dir)
  copy(client_sample_files, client_dir)



if __name__ == "__main__":
  main()