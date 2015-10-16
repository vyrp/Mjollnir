import ast
import datetime
import dbmanager
import os
import shutil
import sys
import traceback
from subprocess import call, check_call, Popen, STDOUT
from time import sleep, time
from uuid import uuid4

sys.path.append('/Mjollnir/vigridr/src/')
from change_game_code import change_game_code
sys.path.pop(-1)

class NullLogger():
    def info(self, msg):
        pass

class ExecutionError(Exception):
    pass

class SiidNullError(Exception):
    pass

class Game():
    def __init__(self, siids, uids, cid, pid, tid, logger):
        self.exts = []
        # Transforming a string of fa list back to a list
        self.siids = ast.literal_eval(siids)
        self.uids = ast.literal_eval(uids)
        self.pid = pid
        self.logger = logger
        self.mid = str(uuid4())
        self.tid = tid
        self.game = '/sandboxes/game-' + self.mid
        self.result = {
            'cid': cid,
            'datetime': datetime.datetime.utcnow(),
            'mid': self.mid,
            'users': []
        }

        if self.tid:
            self.result['tid'] = tid

        # I'm considering here that uids and siids won't be long lists
        # I don't believe we'll have thousands of players in a match
        for uid, siid in zip(self.uids, self.siids):
            self.result['users'].append(
                {
                    'uid': uid,
                    'siid': siid,
                    'rank': -1
                }
            )

        self.logger.info("mid: " + self.mid)

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
        for siid in self.siids:
            self.exts.append(dbmanager.download(siid))

    def compile(self):
        os.chdir('/Mjollnir/vigridr/src/')
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())

        os.chdir('/Mjollnir/vigridr/')
        os.mkdir(self.game + '/')
        with open(os.devnull, "w") as dev_null:
            for idx, siid, ext in zip(range(1, len(self.siids) + 1), self.siids, self.exts):
                lang = 'csharp' if ext == 'cs' else ext
                shutil.move('/sandboxes/downloads/' + siid, 'src/client/ClientLogic.' + ext)
                self.logger.info('make client' + lang)
                check_call(['make', 'client' + lang], stdout=dev_null, stderr=STDOUT)
                shutil.copytree('bin/' + lang, self.game + '/client' + str(idx))
                check_call(['make', 'remove'], stdout=dev_null, stderr=STDOUT)

        os.mkdir(self.game + '/server/')
        shutil.copy('/Mjollnir/vigridr/src/games/' + self.pid + '/bin/server', self.game + '/server/server')

    def run(self):
        try:
            # Construction of server parameters
            os.chdir(self.game)
            server_kwargs = {
                'args': ['./server'],
                'cwd': 'server',
                'stdout': open('server/result', 'w'),
                'stderr': open('server/output', 'w'),
            }
            for idx, uid in enumerate(self.uids):
                server_kwargs['args'].append('--player' + str(idx + 1))
                server_kwargs['args'].append(uid)
            for idx in range(len(self.uids)):
                server_kwargs['args'].append('--port' + str(idx + 1))
                server_kwargs['args'].append('909' + str(idx))

            # Hack for the case of just one player
            # TODO: Fix it!
            if len(self.uids) == 1:
                server_kwargs['args'] = ['./server', '--player1', self.uids[0], '--player2', self.uids[0], '--port1', '9090', '--port2', '9091']

            # Construction of client parameters
            client_kwargs = []
            for idx in range(len(self.uids)):
                client_kwargs.append({
                    'args': ['./client', '--port', '909' + str(idx)],
                    'cwd': 'client' + str(idx + 1),
                    'stdout': open('client' + str(idx + 1) + '/output', 'w'),
                    'stderr': STDOUT,
                })

            # Hack for the case of just one player
            # TODO: Fix it!
            if len(self.uids) == 1:
                client_kwargs.append({
                    'args': ['./client', '--port', '9091'],
                    'cwd': 'client1',
                    'stdout': open('client1/output_', 'w'),
                    'stderr': STDOUT,
                })

            # Executing server
            self.logger.info('Starting server')
            server_process = Popen(**server_kwargs)
            sleep(0.5)

            # Executing clients
            client_processes = []
            for idx, client_kwarg in enumerate(client_kwargs):
                self.logger.info('Starting client' + str(idx + 1))
                client_processes.append(Popen(**client_kwarg))
                sleep(0.5)

            for client_process in client_processes:
                client_process.wait()

            errors = []
            if server_process.wait() != 0:
                errors.append('server(%d)' % server_process.returncode)

        finally:
            server_kwargs['stdout'].close()
            server_kwargs['stderr'].close()

            for client_kwarg in client_kwargs:
                client_kwarg['stdout'].close()

        # TODO: write better error message
        counter = 1
        for client_process, siid in zip(client_processes, self.siids):
            if client_process.returncode != 0:
                dbmanager.upload_runtime_error(siid)
                errors.append('client%d(%d)' % (counter, client_process.returncode))
                counter += 1

        if errors:
            raise ExecutionError('Failed to execute: ' + ' '.join(errors))

        winner = ''
        with open('server/result', 'r') as result:
            winner = result.read()

        self.logger.info("raw winner: " + winner)
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

        if self.ext == 'cs' or self.ext == 'cpp' or self.ext == 'java':
            shutil.move('/sandboxes/downloads/' + self.siid, 'src/client/ClientLogic.' + self.ext)
            self.logger.info('make client' + lang)

            with open(os.devnull, 'w') as dev_null:
                with open(BUILD_ERR, 'w') as build_err:
                    returncode = call(['make', 'client' + lang], stdout=dev_null, stderr=build_err)

                if returncode == 0:
                    self.result = { 'status': 'Success' }
                    self.logger.info('Compilation succeded')
                else:
                    self.logger.info('Compilation failed')
                    self.result = {
                        'status': 'Failure',
                        'error': open(BUILD_ERR, 'r').read()
                    }
                check_call(['make', 'remove'], stdout=dev_null, stderr=dev_null)

        elif self.ext == 'py':
            filename = BUILD + 'ClientLogic.' + self.ext
            shutil.move('/sandboxes/downloads/' + self.siid, filename)

            try:
                self.logger.info('compile python')
                compile(open(filename, 'r').read(), filename, 'exec')
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
