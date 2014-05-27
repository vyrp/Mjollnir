#!/usr/bin/python

import json
import logging
import manager
import os
import signal
import sys
import time
from flask import Flask, request
from logging.handlers import TimedRotatingFileHandler

app = Flask(__name__)
handler = TimedRotatingFileHandler('/Mjollnir/yggdrasil/server/logs/server.log', when='midnight', backupCount=7)
logger = logging.getLogger('werkzeug')

logger.setLevel(logging.INFO)
logger.addHandler(handler)
app.logger.setLevel(logging.INFO)
app.logger.addHandler(handler)

@app.route('/run', methods=['POST'])
def run_handler():
    requires = ['siid1', 'siid2', 'uid1', 'uid2', 'cid']

    for item in requires:
        if not request.form[item]:
            return json.dumps({
                'status': 'error',
                'error': 'missing ' + item
            })

    response = json.dumps(manager.run(*[request.form[item] for item in requires]))
    logger.info('(%s, %s, %s) => %s' % (request.form['siid1'], request.form['siid2'], request.form['cid'], response))
    return response

@app.route('/games')
def games_handler():
    return json.dumps(manager.games())

@app.errorhandler(404)
def not_found(error):
    return json.dumps({
        'status': 'error',
        'error': '404'
    }), 404

@app.errorhandler(500)
def server_error(error):
    return json.dumps({
        'status': 'error',
        'error': '500'
    }), 500

def signal_handler(sig, frame):
    if sig == signal.SIGINT:
        sig = "SIGINT"
    elif sig == signal.SIGTERM:
        sig = "SIGTERM"
    else:
        sig = str(sig)

    logger.warn('=== Process stopped (%s) at %s ===' % (sig, time.strftime('%H:%M:%S')))
    manager.kill()
    sys.exit(-1)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    os.setgid(1000)
    os.setuid(1000)
    app.run(host='127.0.0.1', port=30403)
