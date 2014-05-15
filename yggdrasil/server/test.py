#!/usr/bin/python

from manager import run, kill
from time import sleep

sleep(2)
print 'Testing...'
print run('123', '456', '789')
print run('abc', 'def', 'ghi')
print run('XXX', 'YYY', 'ZZZ')
print 'Testing end'

sleep(20)
kill()
