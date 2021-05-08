import unittest
from unittest.mock import patch
from src.checkers.frozen_checker import FrozenChecker
from src.channel import Channel
from src.constants import FROZEN


CHANNEL_MANAGER = "src.checkers.frozen_checker.ChannelManager"


class TestFrozenChecker(unittest.TestCase):
    @patch(CHANNEL_MANAGER)
    def test_when_channel_with_no_events_should_make_frozen_true(self, mgr):
        # Given
        checker = FrozenChecker()
        channel = Channel("c", [], -1)
        mgr.return_value.get_channels.return_value = [channel]
        # When
        checker.run()
        # Then
        self.assertTrue(channel.get_flag(FROZEN))

    @patch(CHANNEL_MANAGER)
    def test_when_channel_with_events_should_keep_frozen_false(self, mgr):
        # Given
        checker = FrozenChecker()
        channel = Channel("c", ["event"], -1)
        mgr.return_value.get_channels.return_value = [channel]
        # When
        checker.run()
        # Then
        self.assertFalse(channel.get_flag(FROZEN))

    @patch(CHANNEL_MANAGER)
    def test_when_channel_with_events_should_flip_frozen_false(self, mgr):
        # Given
        checker = FrozenChecker()
        channel = Channel("c", ["event"], -1)
        channel.add_flag(FROZEN, True)
        mgr.return_value.get_channels.return_value = [channel]
        # When
        checker.run()
        # Then
        self.assertFalse(channel.get_flag(FROZEN))

    @patch(CHANNEL_MANAGER)
    def test_when_multi_channel_with_events_should_handle_each_case(self, mgr):
        # Given
        checker = FrozenChecker()
        channel = Channel("c", ["event"], -1)
        channel.add_flag(FROZEN, True)
        channel_2 = Channel("c2", [], -1)
        mgr.return_value.get_channels.return_value = [channel, channel_2]
        # When
        checker.run()
        # Then
        self.assertFalse(channel.get_flag(FROZEN))
        self.assertTrue(channel_2.get_flag(FROZEN))
