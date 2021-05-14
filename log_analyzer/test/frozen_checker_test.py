import unittest
from unittest.mock import Mock

from src.channel import Channel
from src.checkers.frozen_checker import FrozenChecker
from src.constants import FROZEN


class TestFrozenChecker(unittest.TestCase):
    def setUp(self):
        self.__checker = FrozenChecker()
        self.__checker.set_measure_key("KEY")
        self.__channels_manager = Mock()
        self.__checker.set_manager(self.__channels_manager)

    def test_when_no_events_should_return_true_flag(self):
        # Given
        channel = Channel("A", [], -1)
        self.__channels_manager.get_channels.return_value = [channel]
        # When
        self.__checker.run()
        # Then
        self.assertTrue(channel.get_flag(FROZEN))

    def test_when_events_should_return_false_flag(self):
        # Given
        channel = Channel("A", [1], -1)
        self.__channels_manager.get_channels.return_value = [channel]
        # When
        self.__checker.run()
        # Then
        self.assertFalse(channel.get_flag(FROZEN))

    def test_when_multi_channel_should_handle_each(self):
        # Given
        channel1 = Channel("A", [], -1)
        channel2 = Channel("A", [1], -1)
        self.__channels_manager.get_channels.return_value = [channel1, channel2]
        # When
        self.__checker.run()
        # Then
        self.assertTrue(channel1.get_flag(FROZEN))
        self.assertFalse(channel2.get_flag(FROZEN))
