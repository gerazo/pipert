import unittest
from unittest.mock import Mock

from src.channel import Channel
from src.checkers.execution_time_checker import ExecutionTimeChecker
from src.constants import EXECUTION_TIME_THRESHOLD, HIGH_EXECUTION_TIME


class TestDropRateChecker(unittest.TestCase):
    def setUp(self):
        self.__checker = ExecutionTimeChecker()
        self.__checker.set_measure_key("KEY")
        self.__channels_manager = Mock()
        self.__checker.set_manager(self.__channels_manager)

    def test_when_measurement_less_should_return_false_flag(self):
        # Given
        channel = self.create_channel('A', 0.5)
        self.__checker.set_parameters({EXECUTION_TIME_THRESHOLD: 0.6})
        self.__channels_manager.get_channels.return_value = [channel]
        # When
        self.__checker.run()
        # Then
        self.assertFalse(channel.get_flag(HIGH_EXECUTION_TIME))

    def test_when_measurement_bigger_should_return_true_flag(self):
        # Given
        channel = self.create_channel('A', 0.5)
        self.__checker.set_parameters({EXECUTION_TIME_THRESHOLD: 0.4})
        self.__channels_manager.get_channels.return_value = [channel]
        # When
        self.__checker.run()
        # Then
        self.assertTrue(channel.get_flag(HIGH_EXECUTION_TIME))

    def test_when_multi_channel_should_handle_each(self):
        # Given
        channel1 = self.create_channel('A', 0.5)
        channel2 = self.create_channel('A', 0.3)
        self.__checker.set_parameters({EXECUTION_TIME_THRESHOLD: 0.4})
        self.__channels_manager.get_channels.return_value = [channel1, channel2]
        # When
        self.__checker.run()
        # Then
        self.assertTrue(channel1.get_flag(HIGH_EXECUTION_TIME))
        self.assertFalse(channel2.get_flag(HIGH_EXECUTION_TIME))

    @staticmethod
    def create_channel(name, measure_val):
        channel = Channel(name, [], -1)
        channel.add_measure("KEY", (1, measure_val))

        return channel
