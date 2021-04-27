import unittest
from src.channel import Channel
from src.channel_calc import ChannelCalc
from src.constants import (FROZEN, HIGH_DROP_RATE, HIGH_DROP_RATIO,
                           HIGH_EXECUTION_TIME)


class TestChannelCalc(unittest.TestCase):

    def test_get_dict(self):
        # Given
        c = Channel("c", [], 12)
        c.update_flag(FROZEN, True)
        cc = ChannelCalc(c)
        # When
        c_dict = cc.get_dict()
        # Then
        flags_names = [FROZEN, HIGH_DROP_RATE, HIGH_DROP_RATIO,
                       HIGH_EXECUTION_TIME]
        flags = {flag: c.get_flag(flag) for flag in flags_names}
        exp_dict = {"name": c.get_name(), "flags": flags, 'DROP_RATE': -1,
                    'EXECUTION_TIME': -1}
        self.assertEqual(c_dict, exp_dict)
