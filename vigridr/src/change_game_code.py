import sys
import shutil as sh
import os
import glob

games_dir = './games'
server_dir = './server'
client_dir = './client'
test_dir = './test'
thrifts_dir = './thrifts'
thrift_compiled_file = '../obj/thrift'
obj_dir = '../obj'
bin_dir = '../bin'

def touch(path):
    with open(path, 'w'):
        os.utime(path, None)

def copy(files, dest_dir):
  for file in files:
    if os.path.isfile(file):
      print "Copying ", file, " to ", dest_dir
      sh.copy(file, dest_dir)

def copy_gen(files, dest_dir):
  for file in files:
    if not os.path.isfile(file) and "gen-" in file:
      directory = os.path.join(dest_dir, os.path.basename(file))
      try:
        sh.rmtree(directory)
      except:
        pass
      os.makedirs(directory, 0777);
      copy(glob.glob(os.path.join(file, '*')), directory)
      
def copy_force(files, dest_dir):
  for file in files:
    if os.path.isfile(file):
      old = os.path.join(dest_dir, os.path.basename(file))
      try:
        os.remove(old)
      except:
        pass
      copy([file], dest_dir)
    else:
      directory = os.path.join(dest_dir, os.path.basename(file))
      try:
        sh.rmtree(directory)
      except:
        pass
      
      os.makedirs(directory, 0777);
      copy_force(glob.glob(os.path.join(file, '*')), directory)
      


def change_game_code(game, copy_sample_clients, copy_tests, copy_obj):
  if game not in os.listdir(games_dir):
    print 'The game "' + game + '" is not in games folder'
    return 1

  game_dir = os.path.join(games_dir, game)

  server_cpp_files = glob.glob(os.path.join(game_dir, "*.cpp"))
  server_h_files = glob.glob(os.path.join(game_dir, "*.h"))
  thrift_files = glob.glob(os.path.join(game_dir, "*.thrift"))
  gen_files = glob.glob(os.path.join(game_dir, "gen-*"))
  obj_files = glob.glob(os.path.join(game_dir, "obj/*"))

  copy(server_cpp_files, server_dir)
  copy(server_h_files, server_dir)
  
  if copy_sample_clients:
    client_sample_files = glob.glob(os.path.join(game_dir, "sampleclient/*"))
    copy(client_sample_files, client_dir)

  if copy_tests:
    test_files = glob.glob(os.path.join(game_dir, "test/*"))
    copy(test_files, test_dir)

  # The thrift commands are sorted to make it look as if the thrift 
  # has been compiled (don't change this order)
  copy(thrift_files, thrifts_dir)  
  if copy_obj:
    touch(thrift_compiled_file)
  copy_gen(gen_files, thrifts_dir)

  if copy_obj:
    copy_force(obj_files, obj_dir)

  return 0


if __name__ == "__main__":
  change_game_code(sys.argv[1], \
                   "--with-client" in sys.argv, \
                   "--with-test" in sys.argv, \
                   "--with-obj" in sys.argv)