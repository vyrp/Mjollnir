from os import environ
from functools import partial
from glicko import GlickoPlayer, RD_factor
from pymongo import MongoClient
from copy import copy

MONGOLAB_URI = environ.get('MONGOLAB_URI')

mongo_client = MongoClient(MONGOLAB_URI)
mongodb = mongo_client['mjollnir-db']

def player_before_match(submission, match):
    """
    Returns a GlickoPlayer corresponding to this submission, taking into 
    account the inactivity period between matches.
    """
    player = GlickoPlayer(submission['rating'], submission['RD'])
    inactivity = (match['datetime'] - submission.get('last_update', match['datetime'])).total_seconds()
    player.advance_periods(inactivity)
    return player

def sign(x):
    if x > 0: return 1
    elif x == 0: return 0
    return -1
    
def process_matches():
    """ 
    Changes rankings according to all of the match results that are on the database,
    but have not been processed since the last run. 
    """

    #find unprocessed matches
    jotunheim_info = mongodb.jotunheim.find_one(sort=[('last_processed', -1)])
    last_processed = jotunheim_info['last_processed']
    unprocessed_matches = mongodb.matches.find({'datetime': {'$gt': last_processed}})

    for match in unprocessed_matches:
        last_processed = max(last_processed, match['datetime'])

        #get submissions for every player in the match
        match_ranks = [match['users']]
        match_submissions = []
        for user in match['users']:
            params = {'cid': match['cid'], 'uid': user['uid']}
            sub = mongodb.submissions.find_one(params)
            match_submissions.append(sub)

        players_before_match = [player_before_match(sub, match) for sub in match_submissions]
        players_after_match = [ copy(player) for player in players_before_match ]

        #update all submissions according to match results
        for sub in match_submissions:
            sub['last_update'] = match['datetime']

        for player_index in xrange(len(match['users'])):
            results = {}
            for other_player in xrange(len(match['users'])):
                if player_index == other_player: continue

                normalized_result = (sign(match['users'][other_player]['rank'] - match['users'][player_index]['rank']) + 1) / 2.0
                results[ players_before_match[other_player] ] = normalized_result

            players_after_match[player_index].match_update(results)
            match_submissions[player_index]['rating'] = players_after_match[player_index].rating
            match_submissions[player_index]['RD'] = players_after_match[player_index].RD     
            mongodb.submissions.save( match_submissions[player_index] )
        
    jotunheim_info['last_processed'] = last_processed
    mongodb.jotunheim.save(jotunheim_info)
    return last_processed

def decrease_old_ratings(last_processed):
    """
    Decrease ratings for players according to their inactivity.
    """
    for sub in mongodb.submissions.find():
        t = last_processed - sub.get('last_update', last_processed)
        
        player = GlickoPlayer(sub['rating'], sub['RD'])
        player.advance_periods(t.total_seconds())
        
        sub['rating'] = player.rating
        sub['RD'] = player.RD
        sub['last_update'] = last_processed
        mongodb.submissions.save(sub)

def submission_priority(sub):
    """
    Returns the priority with which this submission should be matched by the matchmaking system.
    """
    if sub['RD'] > 100:
        return 1000000000 + sub['rating']
    return sub['rating'] + 10*sub['RD']


def match_quality(sub_player, sub_opp):
    """
    Returns a number that represents the value of the match to the matchmaker.
    """
    player = GlickoPlayer(sub_player['rating'], sub_player['RD'])
    opp = GlickoPlayer(sub_opp['rating'], sub_opp['RD'])
    return RD_factor(player, [opp]) 

def execute_matchmaking():
    """
    Based on the current set of ratings, suggests matches to be performed by the server
    to increase reliability of the standings.
    """
    suggested_matches = []

    for challenge in mongodb.challenges.find():
        subs = list(mongodb.submissions.find({'cid': challenge['cid']}))
        subs.sort(key = submission_priority, reverse = True)
        for index, sub in enumerate(subs[:5]):
            best_opponent = max(subs[:index] + subs[index+1:], key=partial(match_quality, sub))
            suggested_matches.append((challenge['cid'], sub['uid'], best_opponent['uid']))
    
    #communicate with Yggdrasil here (API TBD)
    print suggested_matches

def main():
    last_processed = process_matches()
    decrease_old_ratings(last_processed)
    execute_matchmaking()

if __name__ == "__main__":
    main()
