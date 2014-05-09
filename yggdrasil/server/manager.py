__all__ = ['run']

import logging
import shutil
import signal
import sys
import time
from logging.handlers import TimedRotatingFileHandler 

handler = TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/manager.log', when='midnight', backupCount=7)
logger = logging.getLogger('yggdrasil')
logger.setLevel(logging.INFO)
logger.addHandler(handler)

DOWNLOADS = '/sandboxes/downloads/'
CLIENT = '/Mjollnir/vigridr/src/client/'

def fetch_solution(uid):
    logger.info('Fetching solution for ' + uid)
    # Fetch from S3
    # Put it in DOWNLOAD
    # Return file name
    return 'solution' + uid, None
    
def fetch_problem(pid):
    logger.info('Fetching source for problem ' + pid)
    return 'problem' + pid, None

def error_response(error):
    return {
        'status': 'error',
        'error': error
    }

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
    
    # shutil.move(DOWNLOADS + solution1, CLIENT + solution1)
    # compile
    # move
    
    logger.info('Running ' + solution1 + ' against ' + solution2 + ' in ' + problem + '\n')
    
    return {
        'status': 'ok'
    }

if __name__ == '__main__':
    run('100', '101', '1000')
    run('104', '103', '1000')
    run('102', '103', '1001')
