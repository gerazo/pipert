import unittest
from src.utils import flatten_list


class TestUtils(unittest.TestCase):
    def test_flatten_list(self):
        # Given
        listOfLists = [[1, 2], [3, 4], [5, 6]]
        # When
        ret = flatten_list(listOfLists)
        # Then
        exp = [1, 2, 3, 4, 5, 6]
        self.assertEqual(exp, ret)
