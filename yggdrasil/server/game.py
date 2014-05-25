__all__ = ['Game']

import dbmock as dbmanager
import pdb

import os
import shutil
import sys
import threading
import traceback
from time import sleep

sys.path.append('/Mjollnir/vigridr/src/')
from change_game_code import change_game_code

class CommandThread(threading.Thread):
    def __init__(self, command):
        threading.Thread.__init__(self)
        self.result = -1
        self.command = command

    def run(self):
        self.result = os.system(self.command)

class ExecutionError(Exception):
    pass

def execute(command):
    result = os.system(command)
    if result != 0:
        raise ExecutionError('Return of "%s" was %d.' % (command, result))

class Game():
    _counter = 0
    _lock = threading.Lock()
    
    def __init__(self, siid1, siid2, pid, logger):
        self.siid1 = siid1
        self.siid2 = siid2
        self.pid = pid
        self.logger = logger
        with Game._lock:
            Game._counter += 1
            self.game = '/sandboxes/game' + str(Game._counter)
        
    def __enter__(self):
        return self
        
    def __exit__(self, t, v, tr):
        shutil.rmtree(self.game + '/', True)
        if t:
            self.logger.error('From Game.__exit__:')
            for line in traceback.format_exception(t, v, tr):
                self.logger.error(line[:-1])
        return True

    def download(self):
        dbmanager.download(self.siid1)
        dbmanager.download(self.siid2)

    def compile(self):
        os.chdir('/Mjollnir/vigridr/src/')
        change_game_code(self.pid, False, False, False)
        
        os.chdir('/Mjollnir/vigridr/')
        os.mkdir(self.game + '/')
        for idx, siid in [(1, self.siid1), (2, self.siid2)]:
            shutil.move('/sandboxes/downloads/' + siid, 'src/client/ClientLogic.cpp')
            execute('make clientcpp')
            os.mkdir(self.game + '/client' + str(idx))
            shutil.move('bin/cpp/client', self.game + '/client' + str(idx) + '/client')
            execute('make remove')
        
        os.mkdir(self.game + '/server/')
        shutil.copy('/Mjollnir/vigridr/src/games/' + self.pid + '/bin/server', self.game + '/server/server')

    def run(self):
        game_name = self.game.split('/')[-1]
        self.server_log = game_name + '.log'
        self.client1_log = game_name + '.' +  self.siid1 + '.log'
        self.client2_log = game_name + '.' +  self.siid2 + '.log'
        
        os.chdir(self.game)
        server = CommandThread('server/server --port1 9090 --port2 9091 &> ' + server_log)
        client1 = CommandThread('client1/client --port 9090 &> ' + client1_log)
        client2 = CommandThread('client2/client --port 9091 &> ' + client2_log)
        
        pdb.set_trace()
        
        server.start()
        sleep(1)
        client1.start()
        client2.start()
        
        client2.join()
        client1.join()
        server.join()
        
        print '===================> Returns = server(%d) client1(%d) client2(%d)' % (server.result, client1.result, client2.result)

    def upload(self):
        dbmanager.upload(self.game + '/' + self.server_log)
        dbmanager.upload(self.game + '/' + self.client1_log)
        dbmanager.upload(self.game + '/' + self.client2_log)
