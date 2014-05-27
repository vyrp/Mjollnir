__all__ = ['run', 'kill', 'games']

import json
import logging
import threading
import time
from game import Game
from logging.handlers import TimedRotatingFileHandler
from Queue import Empty, Queue
from time import sleep

def now():
    return time.strftime('%H:%M:%S')

def setup_game_names():
    names = {}
    with open('/Mjollnir/vigridr/src/games/names.json', 'r') as file:
        names = json.loads(file.read())
    return names

class GamesManager(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.logger = logging.getLogger('yggdrasil')
        self.logger.setLevel(logging.INFO)
        self.logger.addHandler(TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/manager.log', when='midnight', backupCount=7))
        self.games_queue = Queue()
        self.completed_queue = Queue()
        self.running = True
        self.game_names = setup_game_names()

    def run(self):
        try:
            self.logger.info('[%s] === Starting game queue ===' % (now(), ))
            while self.running:
                siid1, siid2, uid1, uid2, cid, stop = self.games_queue.get() # wait for a requested game
                if stop:
                    break

                pid = self.game_names[cid]

                self.logger.info('[%s] Starting game %s vs %s in %s' % (now(), siid1, siid2, pid))
                with Game(siid1, siid2, uid1, uid2, cid, pid, self.logger) as game:
                    game.download()
                    game.compile()
                    game.run()
                    game.upload()
                    self.completed_queue.put_nowait(game.result)
                self.logger.info('[%s] Ended game %s vs %s in %s' % (now(), siid1, siid2, pid))
                
            self.logger.info('[%s] === Game queue stopped  ===' % (now(), ))
        except Exception as e:
            self.logger.info('[%s] === Game queue interrupted ===\n%s' % (now(), str(e)))

    def run_game(self, siid1, siid2, uid1, uid2, cid):
        if not self.is_alive() or not self.running:
            return {
                'status': 'error',
                'error': 'Game thread not running'
            }

        if cid not in self.game_names:
            self.logger.warn("[%s] cid %s doesn't exist" % (now(), cid))
            return {
                'status': 'error',
                'error': "cid %s doesn't exist" % (cid,)
            }

        pid = self.game_names[cid]

        try:
            self.games_queue.put((siid1, siid2, uid1, uid2, cid, False))
            self.logger.info('[%s] Enqueued game %s vs %s in %s' % (now(), siid1, siid2, pid))
            return {
                'status': 'ok'
            }
        except Exception as e:
            self.logger.info('[%s] Could not enqueue %s vs %s in %s\n%s' % (now(), siid1, siid2, cid, str(e)))
            return {
                'status': 'error',
                'error': 'Could not enqueue the requested game'
            }

    def kill(self):
        self.running = False
        self.games_queue.put((None, None, None, None, None, True))
        
    def games(self):
        completed = []
        try:
            while True:
                completed.append(self.completed_queue.get_nowait())
        except Empty:
            pass
        return completed


manager = GamesManager()
manager.start()

def run(siid1, siid2, uid1, uid2, cid):
    return manager.run_game(siid1, siid2, uid1, uid2, cid)

def kill():
    manager.kill()

def games():
    return manager.games()
