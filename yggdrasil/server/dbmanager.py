from bson.errors import InvalidStringData
from datetime import datetime
from os import environ
from pymongo import MongoClient
from uuid import uuid4

DEBUG = str(environ.get('MJOLLNIR_DEBUG')).lower()
DEBUG = (DEBUG == '1' or DEBUG == 'true')

DOWNLOADS = '/sandboxes/downloads/'

mongo_client = MongoClient(environ['MONGOLAB_URI'])
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

class SiidWithMultipleLanguages(Exception):
    pass

if DEBUG:
    import glob
    import shutil

    S3 = '/sandboxes/s3/'

    def download(siid):
        files = glob.glob(S3 + 'solutions/' + siid + '.*')
        l = len(files)
        if l == 0:
            raise SolutionNotFoundError('siid = ' + siid)
        elif l > 1:
            raise SiidWithMultipleLanguages('siid = ' + siid + ', ' + str(l) + ' languages')

        shutil.copy(files[0], DOWNLOADS + siid)
        return extensions[files[0].split('.')[-1]]

    def upload(match, log):
        mongodb.matches.insert(match)
        shutil.copy(log, S3 + 'matches/' + match['mid'])
else:
    import boto
    s3 = boto.connect_s3()
    solutions_bucket = s3.get_bucket('mjollnir-solutions')
    matches_bucket = s3.get_bucket('mjollnir-matches')

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
        mongodb.matches.insert(match)
        key = matches_bucket.new_key(match['mid'])
        key.set_contents_from_filename(log)

def upload_compilation(sid, siid, result):
    submission = mongodb.submissions.find_one({ 'sid': sid })
    
    if submission['build_siid'] != siid:
        return
    
    if result['status'] == 'Success':
        updated_previous_submissions = submission['previous_submissions']
        if submission['siid']:
            updated_previous_submissions.append({'siid': submission['siid']})

        update_document = {
            '$set': {
                'siid': siid,
                'previous_submissions': updated_previous_submissions,
                'RD': max(160, submission['RD']),
                'build_status': 'Success',
                'build_description': '',
                'build_siid': ''
            }
        }
    else:
        update_document = {
            '$set': {
                'build_status': 'Failure',
                'build_description': result['error'][0:1024]
            }
        }
        try:
            mongodb.submissions.update({ 'sid': sid }, update_document)
        except InvalidStringData:
            update_document = {
                '$set': {
                    'build_status': 'Failure',
                    'build_description': 'Unkown error'
                }
            }

    mongodb.submissions.update({ 'sid': sid }, update_document)

def find_siid(sid):
    submission = mongodb.submissions.find_one({ 'sid': sid })
    return submission['build_siid']

def upload_runtime_error(siid):
    update_document = {
        '$set': {
            'runtime_status': 'Failure'
        }
    }
    mongodb.submissions.update({ 'siid': siid }, update_document)
    
def get_game_names():
    names = {}
    for game in mongodb.challenges.find():
        names[game['cid']] = game['visualizer'].split(".")[0]
    return names

