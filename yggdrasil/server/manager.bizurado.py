__all__ = ['run', 'kill']

import logging
import threading
import time
#from game import Game
from logging.handlers import TimedRotatingFileHandler
from Queue import Queue
from time import sleep

logger = logging.getLogger('yggdrasil')
globals = {
    'games_queue': Queue(),
    'queue_thread': None,
    'running': True,
}

class StdoutLogger():
    def info(self, string):
        print string

    def addHandler(self, hander):
        pass

    def setLevel(self, level):
        pass

logger = StdoutLogger()

class QueueThread(threading.Thread):
    def run(self):
        try:
            logger.info('[%s] === Starting game queue ===' % (time.strftime('%H:%M:%S'), ))
            while globals['running']:
                uid1, uid2, pid = globals['games_queue'].get() # wait for a requested game
                logger.info('[%s] Starting game %s vs %s in %s' % (time.strftime('%H:%M:%S'), uid1, uid2, pid))
                #game = Game(uid1, uid2, pid)
                #game.start()
                sleep(5)
                logger.info('[%s] Ended game %s vs %s in %s' % (time.strftime('%H:%M:%S'), uid1, uid2, pid))
            logger.info('[%s] === Game queue stopped  ===' % (time.strftime('%H:%M:%S'), ))
        except Exception as e:
            logger.info('[%s] === Game queue interrupted ===\n%s' % (time.strftime('%H:%M:%S'), str(e)))

def setup():
    logger.info('[%s] <<< Running setup' % (time.strftime('%H:%M:%S'), ))
    logger.setLevel(logging.INFO)
    logger.addHandler(TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/manager.log', when='midnight', backupCount=7))
    globals['queue_thread'] = QueueThread()
    globals['queue_thread'].start()
    logger.info('[%s] Setup finished >>' % (time.strftime('%H:%M:%S'), ))

def run(uid1, uid2, pid):
    if not globals['queue_thread'].is_alive():
        return {
            'status': 'error',
            'error': 'Games queue not running'
        }
    try:
        globals['games_queue'].put((uid1, uid2, pid))
        logger.info('[%s] Enqueued game %s vs %s in %s' % (time.strftime('%H:%M:%S'), uid1, uid2, pid))
        return {
            'status': 'ok'
        }
    except:
        return {
            'status': 'error',
            'error': 'Could not enqueue the requested game'
        }

def kill():
    globals['running'] = False

setup()
print '<manager imported>'
