import unittest

suite = unittest.defaultTestLoader.discover('.')
unittest.TextTestRunner().run(suite)
