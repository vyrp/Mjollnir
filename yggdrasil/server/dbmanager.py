__all__ = ['download', 'upload']

import boto
import os
from datetime import datetime
from pymongo import MongoClient
from uuid import uuid4

DOWNLOADS = '/sandboxes/downloads/'

s3 = boto.connect_s3()
solutions_bucket = s3.get_bucket('mjollnir-solutions')
matches_bucket = s3.get_bucket('mjollnir-matches')

mongo_client = MongoClient(os.environ['MONGOLAB_URI'])
mongodb = mongo_client['mjollnir-db']

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
    key.get_contents_to_filename(DOWNLOADS + siid)
    
    return ext

def upload(match, log):
    match['datetime'] = datetime.fromtimestamp(match['datetime'])
    mongodb.matches.insert(match)
    key = matches_bucket.new_key(match['mid'])
    key.set_contents_from_filename(log)

def upload_compilation(siid, result):
    pass
