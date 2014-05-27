import os
import shutil
from uuid import uuid4

MOCK = "/sandboxes/mock/"
DOWNLOADS = "/sandboxes/downloads/"

if not os.path.isdir(MOCK):
    raise ImportError("Mock folder doesn't exists")

if not os.path.isdir(DOWNLOADS):
    raise ImportError("Downloads folder doesn't exists")

def download(siid):
    shutil.copy(MOCK + siid, DOWNLOADS)

def upload(log):
    id = str(uuid4())
    shutil.copy(log, MOCK + 'log-' + id)
    return id
