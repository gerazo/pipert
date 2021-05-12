import unittest

from src.event import Event
from src.packet import Packet
from src.constants import PACKET_PUSHED
from src.controllers.channels_manager import ChannelsManager


class TestChannelsManager(unittest.TestCase):
    def setUp(self):
        self.cm = ChannelsManager()

    def test_when_add_packet_with_no_pushed_events_should_store_sender_only(self):
        # Given
        p = Packet("A", "B", [])
        # When
        self.cm.add_packet(p)
        # Then
        channels = [c.get_name() for c in self.cm.get_channels()]
        self.assertEqual(channels, ['A'])

    def test_when_add_packet_with_pushed_events_should_store_sender_n_receiver(self):
        # Given
        event = Event("    "+PACKET_PUSHED, 1, 1, 1, 1, 1)
        p = Packet("A", "    B", [event])
        # When
        self.cm.add_packet(p)
        # Then
        channels = [c.get_name() for c in self.cm.get_channels()]
        self.assertEqual(channels, ['A', 'B'])

    def test_when_add_packet_with_pushed_events_should_create_channels_map(self):
        # Given
        event = Event("    " + PACKET_PUSHED, 1, 1, 1, 1, 1)
        p = Packet("A", "    B", [event])
        # When
        self.cm.add_packet(p)
        channels, channels_map = self.cm.get_channels_map()
        # Then
        self.assertEqual(channels, ['A', 'B'])
        self.assertEqual(channels_map, [(1, 0)])

    def test_when_update_map_with_new_channels_should_return_true_for_update_map(self):
        # Given
        event = Event("    " + PACKET_PUSHED, 1, 1, 1, 1, 1)
        p = Packet("A", "    B", [event])
        # When
        self.cm.add_packet(p)
        update_map = self.cm.should_update_map()
        # Then
        self.assertTrue(update_map)
