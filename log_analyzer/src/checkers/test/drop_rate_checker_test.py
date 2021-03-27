import unittest
from unittest.mock import patch, Mock
from src.checkers.drop_rate_checker import DropRateChecker
from src.constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


CHANNEL_MANAGER = "src.checkers.drop_rate_checker.ChannelManager"


class TestDropRateChecker(unittest.TestCase):
    def setUp(self):
        self.checker = DropRateChecker()

    @patch(CHANNEL_MANAGER)
    def test_when_events_and_rate_bigger_should_true_flag(self, mgr):
        # Given
        channel = self.__get_channel([[1, 2, 3], [1, 2, 3, 4]])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, True)

    @patch(CHANNEL_MANAGER)
    def test_when_events_and_rate_smaller_should_false_flag(self, mgr):
        # Given
        channel = self.__get_channel([[1], [1, 2, 3]])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)

    @patch(CHANNEL_MANAGER)
    def test_when_no_execution_or_dropped_packets_should_false_flag(self, mgr):
        # Given
        channel = self.__get_channel([[], []])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)

    @patch(CHANNEL_MANAGER)
    def test_when_no_execution_packets_should_true_flag(self, mgr):
        # Given
        channel = self.__get_channel([[1], []])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, True)

    @patch(CHANNEL_MANAGER)
    def test_when_no_dropped_packets_should_false_flag(self, mgr):
        # Given
        channel = self.__get_channel([[], [1]])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)

    @patch(CHANNEL_MANAGER)
    def test_when_multi_channels_should_handle_each_case(self, mgr):
        # Given
        channel = self.__get_channel([[1, 2, 3], [1, 2, 3, 4]])
        channel_2 = self.__get_channel([[1], [1]])
        mgr.return_value.get_channels.return_value = [channel, channel_2]
        self.checker.set_config({DROP_RATE_THRESHOLD: 0.8})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_DROP_RATE, False)
        channel_2.update_flag.assert_called_with(HIGH_DROP_RATE, True)

    @staticmethod
    def __get_channel(vals):
        channel = Mock()
        channel.get_event.side_effect = vals

        return channel
