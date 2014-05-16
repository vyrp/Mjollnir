from os import environ
from functools import partial
from pymongo import MongoClient
from copy import copy
import glicko
import random

MONGOLAB_URI = environ.get('MONGOLAB_URI')

mongo_client = MongoClient(MONGOLAB_URI)
mongodb = mongo_client['mjollnir-db']

def player_before_match(submission, match):
    """
    Returns a glicko.GlickoPlayer corresponding to this submission, taking into 
    account the inactivity period between matches.
    """
    player = glicko.GlickoPlayer(submission['rating'], submission['RD'])
    inactivity = (match['datetime'] - submission.get('last_update', match['datetime'])).total_seconds()
    player.advance_periods(inactivity)
    return player

def sign(x):
    if x > 0: return 1
    elif x == 0: return 0
    return -1
    
def new_ratings(players_before_match, match):
    players_after_match = [ copy(player) for player in players_before_match ]
    
    for player_index in xrange(len(match['users'])):
        results = {}
        for other_player in xrange(len(match['users'])):
            if player_index == other_player: continue

            normalized_result = (sign(match['users'][other_player]['rank'] - match['users'][player_index]['rank']) + 1) / 2.0
            results[ players_before_match[other_player] ] = normalized_result

        players_after_match[player_index].match_update(results)

    return players_after_match

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
        
        #update all submissions according to match results
        for sub in match_submissions:
            sub['last_update'] = match['datetime']

        players_after_match = new_ratings(players_before_match, match)

        for player_index in xrange(len(match['users'])):
            new_rating = players_after_match[player_index].rating
            new_RD = players_after_match[player_index].RD

            match_submissions[player_index]['rating'] = new_rating
            match_submissions[player_index]['RD'] = new_RD     
            match_submissions[player_index].setdefault('rating_after_match', []).append( (match['mid'], new_rating) )
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
        
        player = glicko.GlickoPlayer(sub['rating'], sub['RD'])
        player.advance_periods(t.total_seconds())
        
        sub['rating'] = player.rating
        sub['RD'] = player.RD
        sub['last_update'] = last_processed
        mongodb.submissions.save(sub)

def weighted_choice(choices):
    total = sum(w for c, w in choices)
    r = random.uniform(0, total)
    upto = 0
    for c, w in choices:
        if upto + w >= r:
            return c
        upto += w

def submission_priority(sub):
    """
    Returns the priority with which this submission should be matched by the matchmaking system.
    """
    return sub['RD']


def match_quality(sub_player, sub_opp):
    """
    Returns a number that represents the value of the match to the matchmaker.
    """
    player = glicko.GlickoPlayer(sub_player['rating'], sub_player['RD'])
    opp = glicko.GlickoPlayer(sub_opp['rating'], sub_opp['RD'])
    return 1 - abs(0.5 - glicko.E(player, opp)) 

def execute_matchmaking(how_many=1, challenges=mongodb.challenges, submissions=mongodb.submissions):
    """
    Based on the current set of ratings, suggests matches to be performed by the server
    to increase reliability of the standings.
    """
    suggested_matches = []

    for challenge in challenges.find():
        subs = list(submissions.find({'cid': challenge['cid']}))
        ratings = [sub['rating'] for sub in subs]

        subs.sort(key = submission_priority, reverse = True)
        for index, sub in enumerate(subs[:how_many]):
            #print 'matching ', sub['name']
            #print [(x['name'], match_quality(sub, x)) for x in subs[:index] + subs[index+1:]]
            best_opponent = weighted_choice([(other, match_quality(sub, other)) for other in subs[:index] + subs[index+1:]])
            suggested_matches.append( (challenge['cid'], (sub['uid'], best_opponent['uid'])) )
    
    #communicate with Yggdrasil here (API TBD)
    return suggested_matches

def main():
    last_processed = process_matches()
    decrease_old_ratings(last_processed)
    execute_matchmaking()

if __name__ == "__main__":
    main()
