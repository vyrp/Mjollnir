import boto
import os

DOWNLOADS = '/sandboxes/downloads/'

s3 = boto.connect_s3()

def download(siid):
    if not os.path.exists(DOWNLOADS + siid):
        pass

def upload(log):
    pass
