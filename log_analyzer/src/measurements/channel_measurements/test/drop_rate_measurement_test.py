import unittest
from unittest.mock import Mock

from src.constants import PACKET_DROPPED, EXECTION_TIME
from src.measurements.channel_measurements.drop_rate_measurement import DropRateMeasurement
from src.channel import Channel
from src.event import Event


class TestDropRateMeasurement(unittest.TestCase):
    def setUp(self):
        self.__drop_rate_measure = DropRateMeasurement()
        self.__drop_rate_measure.set_measurement_key("A")
        self.__channels_mgr = Mock()
        self.__drop_rate_measure.set_manager(self.__channels_mgr)

    def test_when_events_exists_should_return_the_rate(self):
        # Given
        channel = self.__get_channel([(PACKET_DROPPED, 3), (EXECTION_TIME, 4)])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__drop_rate_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), 0.75)

    def test_when_events_do_not_exist_should_return_minus_1(self):
        # Given
        channel = self.__get_channel([])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__drop_rate_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), -1)

    def test_when_only_execution_events_should_return_zero(self):
        # Given
        channel = self.__get_channel([(EXECTION_TIME, 4)])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__drop_rate_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), 0)

    def test_when_only_drop_events_should_return_one(self):
        # Given
        channel = self.__get_channel([(PACKET_DROPPED, 4)])
        self.__channels_mgr.get_channels.return_value = [channel]
        # When
        self.__drop_rate_measure.run()
        # Then
        self.assertEqual(channel.get_measure("A"), 1)

    def test_when_multi_channels_should_handle_each_of_them(self):
        # Given
        channel1 = self.__get_channel([(PACKET_DROPPED, 4)])
        channel2 = self.__get_channel([(EXECTION_TIME, 4)])
        channel3 = self.__get_channel([])
        channel4 = self.__get_channel([(PACKET_DROPPED, 3), (EXECTION_TIME, 4)])
        self.__channels_mgr.get_channels.return_value = [channel1, channel2, channel3, channel4]
        # When
        self.__drop_rate_measure.run()
        # Then
        self.assertEqual(channel1.get_measure("A"), 1)
        self.assertEqual(channel2.get_measure("A"), 0)
        self.assertEqual(channel3.get_measure("A"), -1)
        self.assertEqual(channel4.get_measure("A"), 0.75)

    @staticmethod
    def __get_channel(values):
        events = []
        for (event_type, count) in values:
            for i in range(0, count):
                e = Event("LOGA"+event_type, 1, 1, 1, 1, 1)
                events.append(e)

        channel = Channel("channel", events, 11)

        return channel
