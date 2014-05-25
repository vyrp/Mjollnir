import dbmanager
import os
import shutil
import threading
import traceback

class CommandThread(threading.Thread):
    def __init__(self, command):
        self.result = -1
        self.command = command

    def run(self):
        self.result = os.system(self.commaand)

class Game():
    _counter = 0
    _lock = threading.Lock()
    
    def __init__(self, uid1, uid2, pid, logger):
        self.uid1 = uid1
        self.uid2 = uid2
        self.pid = pid
        self.logger = logger
        with Game._lock:
            Game._counter += 1
            self.game = '/sandboxes/game' + str(Game._counter)
        
    def __enter__(self):
        return self
        
    def __exit__(self, t, v, tr):
        shutil.rmtree(self.game + '/')
        for line in traceback.format_exception(t, v, tr):
            self.logger.error(line[:-1])
        return True

    def download(self):
        dbmanager.download(self.uid1)
        dbmanager.download(self.uid2)

    def compile(self):
        os.chdir('/Mjollnir/vigridr/')
        change_game_code(self.pid)
        
        for idx, uid in [(1, self.uid1), (2, self.uid2)]:
            shutil.move('/sandboxes/downloads/' + uid, 'src/client/ClientLogic.cpp')
            os.system('make clientcpp')
            shutil.move('bin/cpp/client', self.game + '/client' + str(idx) + '/client')
            os.system('make remove')
        
        shutil.move('/sandboxes/cache/' + self.pid + '/server', self.game + '/server/server')

    def run(self):
        server = CommandThread(self.game + '/server/server --port1 9090 --port2 9091 &> ' + self.game + '/server/log')
        client1 = CommandThread(self.game + '/client1/client --port 9090 &> ' + self.game + '/client1/log')
        client2 = CommandThread(self.game + '/client2/client --port 9091 &> ' + self.game + '/client2/log')
        
        server.start()
        sleep(1)
        client1.start()
        client2.start()
        
        client2.join()
        client1.join()
        server.join()
        
        #server.result
        #client1.result
        #client2.result

    def upload(self):
        dbmanager.upload(self.game + '/server/log')
        dbmanager.upload(self.game + '/client1/log')
        dbmanager.upload(self.game + '/client2/log')
