import unittest
from unittest.mock import patch, Mock
from src.checkers.drop_rate_checker import DropRateChecker
from src.constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


CHANNEL_MANAGER = "src.checkers.drop_rate_checker.ChannelManager"


class TestDropRateChecker(unittest.TestCase):
    def setUp(self):
        self.checker = DropRateChecker()

    @patch(CHANNEL_MANAGER)
    def test_when_rate_bigger_than_threshold_should_flip_flag_true(self, mgr):
        # Given
        channel = self.__get_channel(0.75)
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, True)

    @patch(CHANNEL_MANAGER)
    def test_when_rate_smaller_than_threshold_should_keep_flag_false(self, mgr):
        # Given
        channel = self.__get_channel(0.75)
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)

    @patch(CHANNEL_MANAGER)
    def test_when_multi_channels_should_handle_each_case(self, mgr):
        # Given
        channel = self.__get_channel(0.75)
        channel_2 = self.__get_channel(0.9)
        mgr.return_value.get_channels.return_value = [channel, channel_2]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)
        channel_2.update_flag.assert_called_with(HIGH_DROP_RATE, True)

    @staticmethod
    def __get_channel(threshold):
        channel = Mock()
        channel.calculate_drop_rate.return_value = threshold

        return channel
