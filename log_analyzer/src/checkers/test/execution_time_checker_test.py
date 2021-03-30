import unittest
from unittest.mock import patch, Mock
from src.checkers.execution_time_checker import ExecutionTimeChecker
from src.constants import HIGH_EXECUTION_TIME, EXECUTION_TIME_THRESHOLD


CHANNEL_MANAGER = "src.checkers.execution_time_checker.ChannelManager"


class TestExecutionTimeChecker(unittest.TestCase):
    def setUp(self):
        self.checker = ExecutionTimeChecker()

    @patch(CHANNEL_MANAGER)
    def test_when_exec_events_time_more_threshould_should_true_flag(self, mgr):
        # Given
        events = self.__get_events([3.5, 5.0])
        channel = self.__get_channel(events)
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({EXECUTION_TIME_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_EXECUTION_TIME, True)

    @patch(CHANNEL_MANAGER)
    def test_when_exec_events_time_less_threshould_should_false_flag(self,
                                                                     mgr):
        # Given
        events = self.__get_events([0.5, 0.5, 0.5])
        channel = self.__get_channel(events)
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({EXECUTION_TIME_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_EXECUTION_TIME, False)

    @patch(CHANNEL_MANAGER)
    def test_when_no_exec_events_should_false_flag(self, mgr):
        # Given
        channel = self.__get_channel([])
        mgr.return_value.get_channels.return_value = [channel]
        self.checker.set_config({EXECUTION_TIME_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_EXECUTION_TIME, False)

    @patch(CHANNEL_MANAGER)
    def test_when_multi_channel_should_handle_each_case(self, mgr):
        # Given
        channel = self.__get_channel([])
        events_2 = self.__get_events([0.5, 0.5, 0.5])
        channel_2 = self.__get_channel(events_2)
        events_3 = self.__get_events([3.5, 5.0])
        channel_3 = self.__get_channel(events_3)
        mgr.return_value.get_channels.return_value = [channel, channel_2,
                                                      channel_3]
        self.checker.set_config({EXECUTION_TIME_THRESHOLD: 0.7})
        # When
        self.checker.run()
        # Then
        channel.update_flag.assert_called_with(HIGH_EXECUTION_TIME, False)
        channel_2.update_flag.assert_called_with(HIGH_EXECUTION_TIME, False)
        channel_3.update_flag.assert_called_with(HIGH_EXECUTION_TIME, True)

    @staticmethod
    def __get_channel(vals):
        channel = Mock()
        channel.get_event.return_value = vals

        return channel

    @staticmethod
    def __get_events(avgs):
        events = []
        for avg in avgs:
            event = Mock()
            event.get_avg.return_value = avg
            events.append(event)

        return events
