import datetime
import dbmanager
import json
import os
import shutil
import sys
import traceback
from subprocess import call, check_call, Popen, STDOUT
from time import sleep, time
from threading import Timer
from uuid import uuid4

path = os.path
VIGRIDR_SRC = '/Mjollnir/vigridr/src'
SANDBOXES = '/sandboxes'
BUILD = path.join(SANDBOXES, 'build')
DOWNLOADS = path.join(SANDBOXES, 'downloads')
BUILD_ERR = path.join(BUILD, 'stderr')

sys.path.append(VIGRIDR_SRC)
from change_game_code import change_game_code
sys.path.pop(-1)

class NullLogger():
    def info(self, msg):
        pass

class ResultError(Exception):
    pass

class SiidNullError(Exception):
    pass

class Game():
    def __init__(self, siids, uids, cid, pid, tid, logger):
        self.logger = logger
        self.exts = []
        # Transforming a string of fa list back to a list
        self.siids = json.loads(siids)
        self.uids = json.loads(uids)
        self.num_players = len(self.uids)
        self.pid = pid
        self.mid = str(uuid4())
        self.tid = tid
        self.game = path.join(SANDBOXES, 'game-' + self.mid)
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
            self.result['users'].append({
                'uid': uid,
                'siid': siid,
                'rank': -1
            })

        self.logger.info("mid: " + self.mid)

    def __enter__(self):
        return self

    def __exit__(self, t, v, tr):
        #shutil.rmtree(self.game, True)
        os.chdir(VIGRIDR_SRC)
        change_game_code('template', True, False, False, NullLogger())
        if t:
            self.logger.error('From Game.__exit__:')
            for line in traceback.format_exception(t, v, tr):
                self.logger.error(line[:-1])
        return True

    def download(self):
        for siid in self.siids:
            self.exts.append(dbmanager.download(siid))

    def compile(self):
        os.chdir(VIGRIDR_SRC)
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())

        # Hack for the case of just one player
        # TODO: Fix it!
        if self.num_players == 1:
            self.num_players = 2
            self.uids.append("COMPUTER")
            self.siids.append("COMPUTER")
            self.exts.append("py")

        os.chdir('..')
        os.mkdir(self.game)
        with open(os.devnull, "w") as dev_null:
            for idx, siid, ext in zip(range(1, self.num_players + 1), self.siids, self.exts):
                lang = 'csharp' if ext == 'cs' else ext
                if siid == "COMPUTER": # 1-player hack
                    shutil.copy(path.join(VIGRIDR_SRC, 'games', self.pid, 'sampleclient', 'ClientLogic.py'), path.join(VIGRIDR_SRC, 'client', 'ClientLogic.' + ext))
                else:
                    shutil.copy(path.join(DOWNLOADS, siid), path.join(VIGRIDR_SRC, 'client', 'ClientLogic.' + ext))
                self.logger.info('make client' + lang)
                check_call(['make', 'client' + lang], stdout=dev_null, stderr=STDOUT)
                shutil.copytree(path.join('bin', lang), path.join(self.game, 'client' + str(idx)))
                check_call(['make', 'remove'], stdout=dev_null, stderr=STDOUT)

        os.mkdir(path.join(self.game, 'server'))
        shutil.copy(path.join(VIGRIDR_SRC, 'games', self.pid, 'bin', 'server'), path.join(self.game, 'server', 'server'))

    def run(self):
        try:
            # Construction of server parameters
            os.chdir(self.game)
            server_kwargs = {
                'args': ['./server'],
                'cwd': 'server',
                'stdout': open(path.join('server', 'result'), 'w'),
                'stderr': open(path.join('server', 'output'), 'w'),
            }
            for idx, uid in enumerate(self.uids):
                server_kwargs['args'].append('--player' + str(idx + 1))
                server_kwargs['args'].append(uid)
            for idx in range(self.num_players):
                server_kwargs['args'].append('--port' + str(idx + 1))
                server_kwargs['args'].append('909' + str(idx))

            # Construction of client parameters
            client_kwargs = []
            for idx in range(self.num_players):
                client_kwargs.append({
                    'args': ['./client', '--port', '909' + str(idx)],
                    'cwd': 'client' + str(idx + 1),
                    'stdout': open(path.join('client' + str(idx + 1), 'output'), 'w'),
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

            killed = False
            def kill_server():
                killed = True
                server_process.kill()
                self.logger.info("Killed server")
            server_timer = Timer(6 * 60, kill_server) # 6 minutes
            server_timer.start()
            server_process.wait()
            server_timer.cancel()

            errors = []
            if server_process.returncode != 0:
                errors.append('server')

        finally:
            server_kwargs['stdout'].close()
            server_kwargs['stderr'].close()

            for client_kwarg in client_kwargs:
                client_kwarg['stdout'].close()

        sleep(0.1)
        for client_process, siid, uid in zip(client_processes, self.siids, self.uids):
            if client_process.poll() is None:
                client_process.kill()
            if client_process.returncode != 0:
                dbmanager.upload_runtime_error(siid)
                errors.append(uid)

        if errors:
            self.logger.info('Errors in: ' + ' '.join(errors))
            self.result['errors'] = errors

        if not kiled:
            winner = open(path.join('server', 'result'), 'r').read()

            self.logger.info("raw winner: " + winner)
            if winner == '-1':
                self.logger.info('Result: tie')
                for user in self.result['users']:
                    user['rank'] = 1
            elif winner[0:3] == '909':
                for idx in range(self.num_players):
                    if idx == int(winner[3:]):
                        self.logger.info('Winner: ' + self.siids[idx])
                        self.result['users'][idx]['rank'] = 1
                    else:
                        self.result['users'][idx]['rank'] = 2
            elif winner[0:2] == 's:':
                for user in self.result['users']:
                    self.logger.info('Score: ' + winner[2:])
                    user['rank'] = int(winner[2:])
            else:
                raise ResultError('Unknown result: ' + winner)

    def upload(self):
        dbmanager.upload(dict(self.result), path.join(self.game, 'server', 'logs'))

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
        os.chdir(VIGRIDR_SRC)
        change_game_code('template', True, False, False, NullLogger())
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
        os.chdir(VIGRIDR_SRC)
        self.logger.info('Changing game code')
        change_game_code(self.pid, False, False, False, NullLogger())

        os.chdir('..')
        lang = 'csharp' if self.ext == 'cs' else self.ext

        if self.ext == 'cs' or self.ext == 'cpp' or self.ext == 'java':
            shutil.copy(path.join(DOWNLOADS, self.siid), path.join('src', 'client', 'ClientLogic.' + self.ext))
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
            shutil.copy(path.join(DOWNLOADS, self.siid), filename)

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

