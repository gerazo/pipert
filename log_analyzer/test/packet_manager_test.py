import unittest
from unittest.mock import patch

from packet import Packet
from packets_manager import PacketsManager

PACKET_DECODER = "packets_manager.PacketDecoder"


class TestPacketManager(unittest.TestCase):

    @patch(PACKET_DECODER)
    def test_when_new_packet_should_be_added(self, decoder):
        # Given
        pm = PacketsManager()
        # When
        pm.add(Packet("dummy", "dummy", []))
        # Then
        self.assertEqual(len(pm.get_packets()), 1)

    @patch(PACKET_DECODER)
    def test_when_new_packets_should_be_added(self, decoder):
        # Given
        pm = PacketsManager()
        decoder.return_value.decode_packet.return_value =
        # When
        pm.add(Packet("dummy", "dummy", []))
        pm.add(Packet("dummy2", "dummy2", []))
        # Then
        print(pm.get_packets())
        self.assertEqual(len(pm.get_packets()), 2)
