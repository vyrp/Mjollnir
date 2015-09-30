import dbmanager
import os
import shutil
import sys
import threading
import traceback
import ast
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

class SiidNullError(Exception):
    pass

def execute(command):
    result = os.system(command)
    if result != 0:
        raise ExecutionError('Return of "%s" was %d.' % (command, result))

class Game():
    def __init__(self, siids, uids, cid, pid, logger):
        self.exts = []
        # Transforming a string of a list back to a list
        self.siids = ast.literal_eval(siids)
        self.uids = ast.literal_eval(uids)
        self.pid = pid
        self.logger = logger
        self.mid = str(uuid4())
        self.game = '/sandboxes/game-' + self.mid
        self.result = {
            'cid': cid,
            'datetime': time(),
            'mid': self.mid,
            'users': []
        }

        # I'm considering here that uids and siids won't be long lists
        for uid, siid in zip(self.uids, self.siids):
            self.result['users'].append(
                {
                    'uid': uid,
                    'siid': siid,
                    'rank': -1
                }
            )
        
    def __enter__(self):
        return self
        
    def __exit__(self, t, v, tr):
        shutil.rmtree(self.game, True)
        if t:
            self.logger.error('From Game.__exit__:')
            for line in traceback.format_exception(t, v, tr):
                self.logger.error(line[:-1])
        return True

    # TODO: download solution for computer in wumpus - ???
    def download(self):
        for siid in self.siids:
            self.exts.append(dbmanager.download(siid))

    def compile(self):
        os.chdir('/Mjollnir/vigridr/src/')
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())
        
        os.chdir('/Mjollnir/vigridr/')
        os.mkdir(self.game + '/')
        for idx, siid, ext in zip(range(1, len(self.siids) + 1), self.siids, self.exts):
            lang = 'csharp' if ext == 'cs' else ext
            shutil.move('/sandboxes/downloads/' + siid, 'src/client/ClientLogic.' + ext)
            self.logger.info('make client' + lang)
            execute('make client' + lang + ' 1> /dev/null 2> /dev/null')
            shutil.copytree('bin/' + lang, self.game + '/client' + str(idx))
            execute('make remove 1> /dev/null 2> /dev/null')
        
        os.mkdir(self.game + '/server/')
        shutil.copy('/Mjollnir/vigridr/src/games/' + self.pid + '/bin/server', self.game + '/server/server')

    def run(self):
        os.chdir(self.game)
        command = 'cd server && ./server '
        for idx, uid in zip(range(len(self.uids)), self.uids):
            command = command + '--player' + str(idx + 1) + ' ' + uid + ' '
        for idx in range(len(self.uids)):
            command = command + '--port' + str(idx + 1) + ' 909' + str(idx) + ' '
        command = command + '1> result 2> /dev/null'

        server = CommandThread(command)
        clients = []

        for idx in range(len(self.uids)):
            clients.append( 
                CommandThread (
                    'cd client' + str(idx + 1) + ' && ./client --port 909' + str(idx) + ' 1> /dev/null 2> /dev/null'
                )
            ) 

        self.logger.info('Starting server')
        server.start()
        sleep(1.5)

        for idx, client in zip(range(len(clients)), clients):
            self.logger.info('Starting client' + str(idx + 1))
            client.start()

        for client in clients[::-1]:
            client.join()

        server.join()
        
        # TODO: write better error message
        for client, siid in zip(clients, self.siids):
            if client.result != 0:
                dbmanager.upload_runtime_error(siid)
                raise ExecutionError(
                    'Failed to execute: client(%d) server(%d)' %
                    (client.result, server.result)
                )

        winner = ''
        with open('server/result', 'r') as file:
            winner = file.read()
        
        if winner == '-1':
            self.logger.info('Result: tie')
            for idx in range(len(self.uids)):
                self.result['users'][idx]['rank'] = 1
        elif winner[0:3] == '909':
            for idx in range(len(self.uids)):
                if idx == int(winner[3:]):
                    self.logger.info('Winner: ' + self.siids[idx])
                    self.result['users'][idx]['rank'] = 1
                else:
                    self.result['users'][idx]['rank'] = 2
        elif winner[0:2] == 's:':
            for idx in range(len(self.uids)):
                self.logger.info('Score: ' + winner[2:])
                self.result['users'][idx]['rank'] = int(winner[2:])  
        else:
            self.logger.info('Result: error')


    def upload(self):
        dbmanager.upload(dict(self.result), self.game + '/server/logs')

BUILD = '/sandboxes/build/'
BUILD_ERR = BUILD + 'stderr'

class Compiler():
    def __init__(self, sid, pid, logger):
        self.sid = sid
        self.pid = pid
        self.logger = logger
        
    def __enter__(self):
        return self
        
    def __exit__(self, t, v, tr):
        shutil.rmtree(BUILD, True)
        os.mkdir(BUILD)
        if t:
            self.logger.error('From Compiler.__exit__:')
            for line in traceback.format_exception(t, v, tr):
                self.logger.error(line[:-1])
        return True

    def download(self):
        self.siid = dbmanager.find_siid(self.sid)
        if not self.siid:
            raise SiidNullError('sid = ' + self.sid)
        self.ext = dbmanager.download(self.siid)

    def compile(self):
        os.chdir('/Mjollnir/vigridr/src/')
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())
        
        os.chdir('/Mjollnir/vigridr/')
        lang = 'csharp' if self.ext == 'cs' else self.ext
        
        if self.ext == 'cs' or self.ext == 'cpp':
            shutil.move('/sandboxes/downloads/' + self.siid, 'src/client/ClientLogic.' + self.ext)
            self.logger.info('make client' + lang)
            
            try:
                execute('make client' + lang + ' 1> /dev/null 2> ' + BUILD_ERR)
                self.result = { 'status': 'Success' }
                self.logger.info('Compilation succeded')
            except ExecutionError as e:
                self.logger.info('Compilation failed')
                with open(BUILD_ERR, 'r') as build_err:
                    self.result = {
                        'status': 'Failure',
                        'error': build_err.read()
                    }
            execute('make remove 1> /dev/null 2> /dev/null')
            
        elif self.ext == 'py':
            filename = BUILD + 'ClientLogic.' + self.ext
            shutil.move('/sandboxes/downloads/' + self.siid, filename)
            contents = ''
            with open(filename, 'r') as file:
                contents = file.read()
            
            try:
                self.logger.info('compile python')
                compile(contents, filename, 'exec')
                self.result = { 'status': 'Success' }
                self.logger.info('Compilation succeded')
            except SyntaxError:
                self.result = {
                    'status': 'Failure',
                    'error': traceback.format_exc()
                }
                self.logger.info('Compilation failed')
        
    def upload(self):
        dbmanager.upload_compilation(self.sid, self.siid, self.result)
