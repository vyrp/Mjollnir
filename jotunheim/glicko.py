"""
Implements the glicko rating system. 
For further details on how the system works and what the rating and RD values mean,
see original description at http://www.glicko.net/glicko/glicko.pdf
"""

import math

INITIAL_RATING = 1500
MIN_RD = 30
MAX_RD = 350

# DECAY_PERIOD is the number of inactive time periods player needs to go from RD1 to RD2
RD1 = 50
RD2 = 100
DECAY_PERIOD = 86400

q = math.log(10) / 400
C = (RD2**2 - RD1**2) / float(DECAY_PERIOD)

def g(RD):
    den = (1 + (3*q*q*RD*RD) / (math.pi**2) )**0.5
    return 1./den

def E(player, opp):
    """ 
    Denotes the probability that player will win the match.
    """
    den = 1 + 10**(-g(opp.RD)*(player.rating-opp.rating)/400)
    return 1./den

def RD_factor(player, opp):
    """
    Returns a constant that gives the decrease factor for player's RD after a match.
    Note that the return value of this function is not the exact absolute RD decrease.
    """
    expected = E(player, opp)
    return q**2 * g(opp.RD)**2 * expected * (1-expected)

def rating_change(score, player, opp):
    """ 
    Returns by how much player's rating should change after the given match. 
    Score is given from player's point of view.
    """
    
    d = RD_factor(player, opp)
    expected = E(player, opp)
    return q * g(opp.RD) * (score - expected) / (1./player.RD**2 + d)

class GlickoPlayer:
    def __init__(self, rating=INITIAL_RATING, RD=MAX_RD):
        """ Creates a new player with given rating and RD. """
        self.rating = rating
        self.RD = RD

    def advance_periods(self, t=1):
        """ Increase rating uncertainty based on number of elapsed time periods. """
        self.RD = min( (self.RD**2 + C * t) ** 0.5, MAX_RD)

    def match_update(self, score, opp):
        """ 
        Updates rating after a match with an opponent.
        The best score for this player is 1, while 0.5 is a draw and 0 is the worst score. 
        """
        if score < 0 or score > 1:
            raise ValueError('score should be between 0 and 1')

        d = RD_factor(self, opp)
        rating_delta = rating_change(score, self, opp)
        
        self.rating = int( self.rating + rating_delta + 0.5 )
        self.RD = int( (1./(1./self.RD**2 + d)) ** 0.5 + 0.5 )
        self.RD = max(self.RD, MIN_RD)
