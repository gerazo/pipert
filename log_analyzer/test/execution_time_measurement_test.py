import unittest
from unittest.mock import Mock

from src.channel import Channel
from src.constants import EXECTION_TIME
from src.event import Event
from src.measurements.channel_measurements.execution_time_measurement import ExecutionTimeMeasurement


class TestExecutionTimeMeasurement(unittest.TestCase):
    def setUp(self):
        self.__execution_time_measure = ExecutionTimeMeasurement()
        self.__execution_time_measure.set_measurement_key("A")
        self.__channels_mgr = Mock()
        self.__execution_time_measure.set_manager(self.__channels_mgr)

    def test_when_no_execution_events_should_return_minus_1(self):
        # Given
        channel = self.__get_channel([])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__execution_time_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), -1)

    def test_when_execution_events_should_return_the_execution_time(self):
        # Given
        channel = self.__get_channel([(EXECTION_TIME, 4)])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__execution_time_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), 5)

    def test_when_multi_channels_should_handle_each_of_them(self):
        # Given
        channel1 = self.__get_channel([(EXECTION_TIME, 4)])
        channel2 = self.__get_channel([])
        self.__channels_mgr.get_channels.return_value = [channel1, channel2]
        # When
        self.__execution_time_measure.run()
        # Then
        self.assertEqual(channel1.get_measure("A"), 5)
        self.assertEqual(channel2.get_measure("A"), -1)

    @staticmethod
    def __get_channel(values):
        events = []
        for (event_type, count) in values:
            for i in range(0, count):
                e = Event("LOGA"+event_type, 1, 1, 1, 1, 5)
                events.append(e)

        channel = Channel("channel", events, 11)

        return channel
