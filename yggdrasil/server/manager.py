__all__ = ['run', 'kill']

import logging
import threading
import time
from logging.handlers import TimedRotatingFileHandler
from Queue import Queue
from time import sleep

def now():
    return time.strftime('%H:%M:%S')

class GamesManager(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.logger = logging.getLogger('yggdrasil')
        self.logger.setLevel(logging.INFO)
        self.logger.addHandler(TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/manager.log', when='midnight', backupCount=7))
        self.games_queue = Queue()
        self.running = True

    def run(self):
        try:
            self.logger.info('[%s] === Starting game queue ===' % (now(), ))
            while self.running:
                uid1, uid2, pid, stop = self.games_queue.get() # wait for a requested game
                if stop:
                    break
                self.logger.info('[%s] Starting game %s vs %s in %s' % (now(), uid1, uid2, pid))
                sleep(5)
                self.logger.info('[%s] Ended game %s vs %s in %s' % (now(), uid1, uid2, pid))
            self.logger.info('[%s] === Game queue stopped  ===' % (now(), ))
        except Exception as e:
            self.logger.info('[%s] === Game queue interrupted ===\n%s' % (now(), str(e)))

    def run_game(self, uid1, uid2, pid):
        if not self.is_alive():
            return {
                'status': 'error',
                'error': 'Game thread not running'
            }
        try:
            self.games_queue.put((uid1, uid2, pid, False))
            self.logger.info('[%s] Enqueued game %s vs %s in %s' % (now(), uid1, uid2, pid))
            return {
                'status': 'ok'
            }
        except Exception as e:
            self.logger.info('[%s] Could not enqueue %s vs %s in %s\n%s' % (now(), uid1, uid2, pid, str(e)))            
            return {
                'status': 'error',
                'error': 'Could not enqueue the requested game'
            }

    def kill(self):
        self.running = False
        self.games_queue.put((None, None, None, True))


manager = GamesManager()
manager.start()

def run(uid1, uid2, pid):
    return manager.run_game(uid1, uid2, pid)

def kill():
    manager.kill()
