import unittest
import glicko

class GlickoTest(unittest.TestCase):
    def setUp(self):
        self.players = [glicko.GlickoPlayer(1500, 200),
                        glicko.GlickoPlayer(1400, 30),
                        glicko.GlickoPlayer(1550, 100),
                        glicko.GlickoPlayer(1700, 300)]

    def test_G(self):
        player = self.players
        self.assertAlmostEqual(glicko.g(player[1].RD), 0.9955, places=4)
        self.assertAlmostEqual(glicko.g(player[2].RD), 0.9531, places=4)
        self.assertAlmostEqual(glicko.g(player[3].RD), 0.7242, places=4)

    def test_E(self):
        player = self.players
        E01 = glicko.E(player[0], player[1])
        E02 = glicko.E(player[0], player[2])
        E03 = glicko.E(player[0], player[3])
        self.assertAlmostEqual(E01, 0.639, places=3)
        self.assertAlmostEqual(E02, 0.432, places=3)
        self.assertAlmostEqual(E03, 0.303, places=3)

    def test_decay_period(self):
        player = glicko.GlickoPlayer(RD=glicko.RD1)
        player.advance_periods(glicko.DECAY_PERIOD)
        self.assertEqual(player.RD, glicko.RD2)

    def test_invalid_score(self):
        self.assertRaises(ValueError, self.players[0].match_update, {self.players[1]: 1.5} )
        self.assertRaises(ValueError, self.players[0].match_update, {self.players[1]: -0.5} )

    def test_update(self):
        self.players[0].match_update({self.players[1]: 1})
        self.assertEqual(self.players[0].rating, 1563)
        self.assertAlmostEqual(self.players[0].RD, 175.22, places=2)

    def test_update_multiple(self):
        self.players[0].match_update({self.players[1]: 1,
                                      self.players[2]: 0,
                                      self.players[3]: 0})
        self.assertEqual(self.players[0].rating, 1464)
        self.assertAlmostEqual(self.players[0].RD, 151.4, places=1)
