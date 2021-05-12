import unittest
from unittest.mock import patch

from src.utils import *

RDP = "src.utils.rdp"


class TestUtils(unittest.TestCase):
    def test_when_flatten_list_should_return_list_with_all_elements(self):
        # Given
        list_of_lists = [[1, 2], [3, 4], [5, 6]]
        # When
        ret = flatten_list(list_of_lists)
        # Then
        exp = [1, 2, 3, 4, 5, 6]
        self.assertEqual(exp, ret)

    def test_when_calculating_average_should_return_average(self):
        # Given
        lst = [1, 2, 3, 4, 5]
        # When
        avg = calc_avg(lst)
        # Then
        self.assertEqual(avg, 3)

    @patch(RDP)
    def test_when_reduce_n_extract_should_return_list_of_y_axis(self, rdp_func):
        # Given
        rdp_func.return_value = [(1, 2), (2, 2), (5, 3)]
        # When
        expected = reduce_points_n_extract_y_axis([])
        # Then
        self.assertEqual([2, 2, None, None, 3, None, None, None, None, None], expected)
