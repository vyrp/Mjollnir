import random
import itertools
import jotunheim
import glicko
from os import environ, system
from uuid import uuid4

# clearly a stub, and not a spy, mock or fake
class StubCollectionFinder(list):
    def find(self, query={}):
        return [x for x in self if all(x[key] == query[key] for key in query)]

challenge_ids = [str(uuid4()) for x in xrange(2)]
user_names = ["Fenrir", "Sleipnir", "Lenneth", "Arngrim", "Loki"]
user_ids = [str(uuid4()) for x in xrange(len(user_names))]    

challenges = [{'cid': cid} for cid in challenge_ids]
submissions = []
for cid in challenge_ids:
    for index, uid in enumerate(user_ids):
        submissions.append({'cid': cid, 'uid': uid, 'rating':1500, 'RD':300, 'name': user_names[index]} )

challenges = StubCollectionFinder(challenges)
submissions = StubCollectionFinder(submissions)

# Real values for (sigma, mu) for our test users, assuming they perform according to a normal distribution
# Final ranking should converge to real ranking as fast as possible.
real_performance = {}
for cid, uid in itertools.product(challenge_ids, user_ids):
    real_performance[(cid,uid)] = (random.randint(1200, 2000), random.randint(10, 30))


def print_rankings(cid):
    print "For challenge %s\n" % cid
    for x in sorted(submissions, key=lambda x: x['rating']):
        if x['cid'] == cid:
            print '%-10s%-10d%5.2f%10s' % (x['name'], x['rating'], x['RD'], real_performance[cid, x['uid']])


def execute_matches(suggested_matches):
    """
    Simulates running the matches by using the real performances of the test users 
    and sending match results through the database.
    """
    for match in suggested_matches:
        cid = match[0]
        performances = [ (random.gauss(*real_performance[cid,uid]), uid) for uid in match[1] ]
        
        performances.sort(reverse=True)
        db_match = {'users': []}
        rank = 1

        for index in xrange(len(performances)):
            if index != 0 and performances[index-1][0] > performances[index][0]:
                rank += 1

            db_match['users'].append({'uid': performances[index][1], 'rank': rank}) 

        players_before_match = []
        
        for index in xrange(len(performances)):
            for sub in submissions:
                if sub['cid'] == cid and sub['uid'] == performances[index][1]:
                    players_before_match.append( glicko.GlickoPlayer(sub['rating'], sub['RD']) )

        players_after_match = jotunheim.new_ratings(players_before_match, db_match)

        for index in xrange(len(performances)):
            for sub in submissions:
                if sub['cid'] == cid and sub['uid'] == performances[index][1]:
                    sub['rating'] = players_after_match[index].rating
                    sub['RD'] = players_after_match[index].RD

matches = 0

while True:
    
    print "State after %d matches:\n\n" % matches
    for challenge in challenge_ids:
        print_rankings(challenge)
        print

    print 'Press Enter for the next iteration...'
    inp = raw_input()
    if inp == 'p':
        uid = uuid4() 
        for cid in challenge_ids:    
            real_performance[cid,uid]=(random.randint(1200, 2000), random.randint(10, 30))
            submissions.append({'cid': cid, 'uid': uid, 'rating':1500, 'RD':300, 'name': 'New User'} )
    elif inp == 'q':
        break

    system('cls')

    suggested_matches = jotunheim.execute_matchmaking(challenges=challenges, submissions=submissions)
    execute_matches(suggested_matches)
    matches += 1
