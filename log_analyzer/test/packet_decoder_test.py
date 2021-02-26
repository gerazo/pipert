import unittest

from packet_decoder import PacketDecoder


class TestPacketDecoder(unittest.TestCase):
    def test_when_wrong_packet_serialization_should_raise_value_err(self):
        # Given
        packet = b'DGRB'
        decoder = PacketDecoder(packet)
        # When & Then
        self.assertRaises(ValueError, decoder.decode_packet)

    def test_when_packet_should_decode(self):
        # Given
        packet = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time\x00\x00\x00\x00\n\x00\x00\x08\xe3?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xe6ffffff' \
                 b'LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00\t8?\xf0\x00\x00\x00\x00\x00\x00@\x8c \x00\x00\x00\x00\x00@\x88\x0733333'

        decoder = PacketDecoder(packet)
        # When
        p, pos = decoder.decode_packet(0)
        # Then
        self.assertEqual("OutChannel", p.get_receiver())

    def test_when(self):
        # Given
        packets = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time\x00\x00\x00\x00\n\x00\x00,Q?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xe3333333LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00,\x8b?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@\x85\xe0\xcc\xcc\xcc\xcc\xcd'b'DGRPReverserChannel\x00SENDPrinterChannel\x00LOGAExecution Time\x00\x00\x00\x00\n\x00\x00+\xe4?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@-333333LOGAPacket Retrieved\x00\x00\x00\x00\n\x00\x00+\xee?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@\x85\xf4\x00\x00\x00\x00\x00LOGARead Time\x00\x00\x00\x00\n\x00\x00+\xf8?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@,333333LOGAFill Time\x00\x00\x00\x00\n\x00\x00,\t?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xec\xcc\xcc\xcc\xcc\xcc\xcdLOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00,\x1f?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@}T\xcc\xcc\xcc\xcc\xcd'b'DGRPPrinterChannel\x00SENDN/A\x00LOGAExecution Time\x00\x00\x00\x00\n\x00\x00+|?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@@\x19\x99\x99\x99\x99\x9aLOGAPacket Retrieved\x00\x00\x00\x00\n\x00\x00+\x86?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@}\x00\x00\x00\x00\x00\x00LOGARead Time\x00\x00\x00\x00\n\x00\x00+\x91?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@@s33333LOGAFill Time\x00\x00\x00\x00\n\x00\x00+\xa2?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@\x03333333LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00+\xb2?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@vK33333'

        decoder = PacketDecoder(packets)
        # When
        p = decoder.decode_packets()
        print(p)
        # Then
        # self.assertEqual("OutChannel", p.get_receiver())
