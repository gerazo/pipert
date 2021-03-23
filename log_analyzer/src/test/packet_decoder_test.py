import unittest

from src.packet_decoder import PacketDecoder


class TestPacketDecoder(unittest.TestCase):
    def test_when_wrong_packet_serialization_should_raise_value_err(self):
        # Given
        packet = b'DGRB'
        decoder = PacketDecoder(packet)
        # When & Then
        self.assertRaises(ValueError, decoder.decode_packet)

    def test_when_packet_should_decode(self):
        # Given
        packet = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time' \
                 b'\x00\x00\x00\x00\n\x00\x00\x08\xe3?\xf0\x00\x00\x00\x00' \
                 b'\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xe6ffffff' \
                 b'LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00\t8?\xf0' \
                 b'\x00\x00\x00\x00\x00\x00@\x8c \x00' \
                 b'\x00\x00\x00\x00@\x88\x0733333'

        decoder = PacketDecoder(packet)
        # When
        p, pos = decoder.decode_packet(0)
        # Then
        self.assertEqual("OutChannel", p.get_receiver())

    def test_when(self):
        # Given
        packets = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time' \
                  b'\x00\x00\x00\x00\n\x00\x00,Q?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00?' \
                  b'\xe3333333LOGAPacket Pushed\x00\
                  x00\x00\x00\n\x00\x00,\x8b?\xf0\x00\x00\x00' \
                  b'\x00\x00\x00?\xf0\x00\x00' \
                  b'\x00\x00\x00\x00@\x85\xe0\xcc\xcc\xcc\xcc' \
                  b'\xcd'b'DGRPReverserChannel' \
                  b'\x00SENDPrinterChannel\x00LOGAExecution Time' \
                  b'\x00\x00\x00\x00\n\x00\x00+' \
                  b'\xe4?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00' \
                  b'\x00\x00\x00\x00@-333333' \
                  b'LOGAPacket Retrieved\x00\x00\x00\x00\n\x00\x00+\xee?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00@' \
                  b'\x85\xf4\x00\x00\x00\x00\x00' \
                  b'LOGARead Time\x00\x00\x00' \
                  b'\x00\n\x00\x00+\xf8?\xf0\x00\x00\x00\x00\x00\x00?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00' \
                  b'@,333333LOGAFill Time\x00\x00\x00\x00\n\x00\x00,\t?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00?' \
                  b'\xf0\x00\x00\x00\x00\x00\x00?\xec\xcc\xcc' \
                  b'\xcc\xcc\xcc\xcd' \
                  b'LOGAPacket Pushed' \
                  b'\x00\x00\x00\x00\n\x00\x00,\x1f?\xf0\x00\x00' \
                  b'\x00\x00\x00\x00?\xf0\x00\x00\x00' \
                  b'\x00\x00\x00@}T\xcc\xcc\xcc\xcc\xcd'b'DGRP' \
                  b'PrinterChannel\x00SENDN/A\x00LOGAExecution Time' \
                  b'\x00\x00\x00\x00\n\x00\x00+|?\xf0\x00\x00\x00\x00\x00' \
                  b'\x00?\xf0\x00\x00\x00\x00\x00\x00@@\x19' \
                  b'\x99\x99\x99\x99\x9aLOGAPacket Retrieved\x00\x00\x00\x00' \
                  b'\n\x00\x00+\x86?\xf0\x00\x00\x00\x00\x00\
                  x00?\xf0\x00\x00\x00\x00\x00\x00@}\x00\x00\x00\x00\x00\x00' \
                  b'LOGARead Time\x00\x00\x00\x00\n\x00\x00+\x91' \
                  b'?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00' \
                  b'\x00\x00\x00\x00' \
                  b'@@s33333LOGAFill Time\x00\x00\x00\x00\n\x00\x00+\xa2?\
                  xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00@' \
                  b'\x03333333LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00+' \
                  b'\xb2?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00' \
                  b'\x00\x00\x00@vK33333'

        decoder = PacketDecoder(packets)
        # When
        p = decoder.decode_packets()
        print(p)
        # Then
        # self.assertEqual("OutChannel", p.get_receiver())
