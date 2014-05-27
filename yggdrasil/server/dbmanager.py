__all__ = ['download', 'upload']

import boto
import os
from uuid import uuid4

DOWNLOADS = '/sandboxes/downloads/'

s3 = boto.connect_s3()
solutions_bucket = s3.get_bucket('mjollnir-solutions')
logs_bucket = s3.get_bucket('mjollnir-log')

extensions = {
    'cpp11': 'cpp',
    'cs40': 'cs',
    'python27': 'py',
}

class SolutionNotFoundError(Exception):
    pass

class SolutionWithoutLanguageError(Exception):
    pass

def download(siid):
    key = solutions_bucket.get_key(siid)
    if not key:
        raise SolutionNotFoundError('siid = ' + siid)

    language = key.get_metadata('language')
    if not language:
        raise SolutionWithoutLanguageError('siid = ' + siid)
    
    ext = extensions[language]
    key.get_contents_to_filename(DOWNLOADS + siid + '.' + ext)

def upload(log):
    siid = str(uuid4())
    key = logs_bucket.new_key(siid)
    key.set_contents_from_filename(log)
    return siid
