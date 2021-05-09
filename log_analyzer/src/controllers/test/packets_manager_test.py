import unittest
from unittest.mock import patch

from src.packet import Packet
from src.controllers.packets_manager import PacketsManager

PACKET_DECODER = "src.controllers.packets_manager.PacketDecoder"


class TestPacketManager(unittest.TestCase):
    @patch(PACKET_DECODER)
    def test_when_add_packet_should_assign_it_to_the_latest_packet(self, decoder):
        # Given
        pm = PacketsManager()
        packet = Packet("dummy", "    dummy", [])
        # When
        decoder.return_value.decode_packet.return_value = packet
        pm.add([])
        # Then
        self.assertEqual(pm.get_latest_packet(), packet)

    @patch(PACKET_DECODER)
    def test_when_add_packet_should_assign_id_for_packet(self, decoder):
        # Given
        pm = PacketsManager()
        packet = Packet("dummy", "    dummy", [])
        # When
        decoder.return_value.decode_packet.return_value = packet
        pm.add([])
        # Then
        self.assertEqual(pm.get_latest_packet().get_id(), 1)

    @patch(PACKET_DECODER)
    def test_when_add_packet_should_increase_packets_count(self, decoder):
        # Given
        pm = PacketsManager()
        packet = Packet("dummy", "    dummy", [])
        # When
        decoder.return_value.decode_packet.return_value = packet
        for i in range(5):
            pm.add(packet)
        # Then
        self.assertEqual(pm.get_packet_count(), 5)
