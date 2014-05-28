__all__ = ['Game']

import dbmock as dbmanager
import os
import shutil
import sys
import threading
import traceback
from time import sleep, time
from uuid import uuid4

sys.path.append('/Mjollnir/vigridr/src/')
from change_game_code import change_game_code

class NullLogger():
    def info(self, msg):
        pass

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
    def __init__(self, siid1, siid2, uid1, uid2, cid, pid, logger):
        self.siid1 = siid1
        self.siid2 = siid2
        self.uid1 = uid1
        self.uid2 = uid2
        self.pid = pid
        self.logger = logger
        self.mid = str(uuid4())
        self.game = '/sandboxes/game-' + self.mid
        self.result = {
            'cid': cid,
            'datetime': time(),
            'log': None,
            'mid': self.mid,
            'users': [{
                'uid': uid1,
                'siid': siid1,
                'rank': -1
            }, {
                'uid': uid2,
                'siid': siid2,
                'rank': -1
            }]
        }
        
    def __enter__(self):
        return self
        
    def __exit__(self, t, v, tr):
        shutil.rmtree(self.game, True)
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
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())
        
        os.chdir('/Mjollnir/vigridr/')
        os.mkdir(self.game + '/')
        for idx, siid in [('1', self.siid1), ('2', self.siid2)]:
            ext = siid.split('.')[-1]
            lang = 'csharp' if ext == 'cs' else ext
            shutil.move('/sandboxes/downloads/' + siid, 'src/client/ClientLogic.' + ext)
            self.logger.info('make client' + lang)
            execute('make client' + lang + ' 1> /dev/null 2> /dev/null')
            shutil.copytree('bin/' + lang, self.game + '/client' + idx)
            execute('make remove 1> /dev/null 2> /dev/null')
        
        os.mkdir(self.game + '/server/')
        shutil.copy('/Mjollnir/vigridr/src/games/' + self.pid + '/bin/server', self.game + '/server/server')

    def run(self):
        os.chdir(self.game)
        server = CommandThread(
            'cd server && ./server --player1 %s --player2 %s --port1 %s --port2 %s 1> result 2> /dev/null' %
            (self.uid1, self.uid2, '9090', '9091')
        )
        client1 = CommandThread('cd client1 && ./client --port %s 1> /dev/null 2> /dev/null' % ('9090',))
        client2 = CommandThread('cd client2 && ./client --port %s 1> /dev/null 2> /dev/null' % ('9091',))

        self.logger.info('Starting server')
        server.start()
        sleep(1)

        self.logger.info('Starting client1')
        client1.start()

        self.logger.info('Starting client2')
        client2.start()
        
        client2.join()
        client1.join()
        server.join()
        
        if client1.result != 0 or client2.result != 0 or server.result != 0:
            raise ExecutionError(
                'Failed to execute: client1(%d) client2(%d) server(%d)' %
                (client1.result, client2.result, server.result)
            )
        
        winner = ''
        with open('server/result', 'r') as file:
            winner = file.read()
        
        if winner == '-1':
            self.logger.info('Result: tie')
            self.result['users'][0]['rank'] = 1
            self.result['users'][1]['rank'] = 1
        elif winner == '9090':
            self.logger.info('Winner: ' + self.siid1)
            self.result['users'][0]['rank'] = 1
            self.result['users'][1]['rank'] = 2
        elif winner == '9091':
            self.logger.info('Winner: ' + self.siid2)
            self.result['users'][0]['rank'] = 2
            self.result['users'][1]['rank'] = 1
        else:
            self.logger.info('Result: error')

    def upload(self):
        self.result['log'] = dbmanager.upload(self.game + '/server/logs')