import os
import shutil

MOCK = "/sandboxes/mock/"
DOWNLOADS = "/sandboxes/downloads/"

if not os.path.isdir(MOCK):
    raise ImportError("Mock folder doesn't exists")

if not os.path.isdir(DOWNLOADS):
    raise ImportError("Downloads folder doesn't exists")

def download(siid):
    if not os.path.exists(DOWNLOADS + siid):
        shutil.copy(MOCK + siid, DOWNLOADS)

def upload(log):
    shutil.copy(log, MOCK)
