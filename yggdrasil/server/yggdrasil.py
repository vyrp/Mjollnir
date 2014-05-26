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
    siid1 = request.form['siid1']
    siid2 = request.form['siid2']
    pid = request.form['pid']
    
    if not siid1:
        return json.dumps({
            'status': 'error',
            'error': 'missing siid1'
        })

    if not siid2:
        return json.dumps({
            'status': 'error',
            'error': 'missing siid2'
        })
    
    if not pid:
        return json.dumps({
            'status': 'error',
            'error': 'missing pid'
        })

    response = json.dumps(manager.run(siid1, siid2, pid))
    logger.info('(%s, %s, %s) => %s' % (siid1, siid2, pid, response))
    return response

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
    app.run(host='0.0.0.0', port=30403)
