from os import environ
from uuid import uuid4
from glicko import GlickoPlayer
from pymongo import MongoClient

MONGOLAB_URI = environ.get('MONGOLAB_URI')

mongo_client = MongoClient(MONGOLAB_URI)
mongodb = mongo_client['mjollnir-db']

def player_before_match(submission, match):
    """
    Returns a GlickoPlayer corresponding to this submission, taking into 
    account the inactivity period between matches.
    """
    player = GlickoPlayer(submission.rating, submission.RD)
    inactivity = (match.datetime - submission1['last_update']).total_seconds()
    player.advance_periods(inactivity)
    return player
    
def process_match_rating(submission, player, opp, score):
    """
    Updates submission on the database to reflect the match played.
    """
    final_player = copy.copy(player)
    final_player.match_update(score, opp)

    submission['rating'] = final_player.rating
    submission['RD'] = final_player.RD
    
    mongodb.submissions.save(submission)

def process_matches():
    """ 
    Changes rankings according to all of the match results that are on the database,
    but have not been processed since the last run. 
    """
    jotunheim_info = mongodb.jotunheim.find_one(sort=[('last_processed', -1)])
    last_processed = jotunheim_info['last_processed']

    unprocessed_matches = mongodb.matches.find({'datetime': {'$gt': last_processed}})

    for match in unprocessed_matches:
        last_processed = max(last_processed, match.datetime)

        submission1 = mongodb.submissions.find_one({'cid': match.cid, 'uid': match.uid1})
        submission2 = mongodb.submissions.find_one({'cid': match.cid, 'uid': match.uid2})

        player1 = player_before_match(submission1, match)
        player2 = player_before_match(submission2, match)

        submission1['last_update'] = submission2['last_update'] = match.datetime
        normalized_result = (result + 1) / 2.0

        process_match_rating(submission1, player1, player2, normalized_result)
        process_match_rating(submission2, player2, player1, 1-normalized_result) 

    jotunheim_info['last_processed'] = last_processed
    mongodb.jotunheim.save(jotunheim_info)


def execute_matchmaking():
    """
    Based on the current set of ratings, suggests matches to be performed by the server
    to increase reliability of the standings.
    """
    pass


def main():
    process_matches()
    execute_matchmaking()

if __name__ == "__main__":
    main()
