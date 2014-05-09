__all__ = ['run']

import logging
import os
import shutil
import signal
import sys
import time
from logging.handlers import TimedRotatingFileHandler 

logger = None
if len(sys.argv) == 2 and sys.argv[1] == 'd':
    class PrintLogger():
        def info(self, msg):
            print msg
    logger = PrintLogger()
else:
    logger = logging.getLogger('yggdrasil')
    logger.setLevel(logging.INFO)
    handler = TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/manager.log', when='midnight', backupCount=7)
    logger.addHandler(handler)

SANDBOXES = '/sandboxes/'
DOWNLOADS = SANDBOXES + 'downloads/'
MOCK = SANDBOXES + 'mock/'
COMPILING = SANDBOXES + 'compiling/'

def download(type, id):
    name = type + id + '.cpp'
    logger.info('Fetching source for ' + name)
    try:
        shutil.copy(MOCK + name, DOWNLOADS + name)
    except IOError as e:
        if e.errno == 2:
            return None, name + ' not found'
        else:
            raise e
    return name, None

def fetch_solution(uid):
    return download('solution', uid)
    
def fetch_problem(pid):
    return download('problem', pid)

def error_response(error):
    return {
        'status': 'error',
        'error': error
    }

def compile(file):
    os.system('g++ -Wall {0} -o {1} && {1}'.format(file, file[0:-4]))

def run(uid1, uid2, pid):
    logger.info('[' + time.strftime('%H:%M:%S') + ']')
    
    solution1, error = fetch_solution(uid1)
    if error:
        return error_response(error)

    solution2, error = fetch_solution(uid2)
    if error:
        return error_response(error)

    problem, error = fetch_problem(pid)
    if error:
        return error_response(error)
    
    shutil.move(DOWNLOADS + solution1, COMPILING)
    shutil.move(DOWNLOADS + solution2, COMPILING)
    shutil.move(DOWNLOADS + problem, COMPILING)

    compile(COMPILING + solution1)
    compile(COMPILING + solution2)
    compile(COMPILING + problem)
    
    logger.info('Running ' + solution1 + ' against ' + solution2 + ' in ' + problem + '\n')
    
    return {
        'status': 'ok'
    }

if __name__ == '__main__':
    run('100', '101', '1000')
    run('104', '103', '1000')
    run('102', '103', '1001')
