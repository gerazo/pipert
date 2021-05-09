import unittest

from src.packet_decoder import PacketDecoder


class TestPacketDecoder(unittest.TestCase):
    def test_when_wrong_packet_serialization_should_raise_value_err(self):
        # Given
        packet = b'DGRB'
        decoder = PacketDecoder(packet)
        # When & Then
        self.assertRaises(ValueError, decoder.decode_packet)

    def test_when_packet_decode_packet_should_store_sender_n_receiver_correctly(self):
        # Given
        packet = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time' \
                 b'\x00\x00\x00\x00\n\x00\x00\x08\xe3?\xf0\x00\x00\x00\x00' \
                 b'\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xe6ffffff' \
                 b'LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00\t8?\xf0' \
                 b'\x00\x00\x00\x00\x00\x00@\x8c \x00' \
                 b'\x00\x00\x00\x00@\x88\x0733333'

        decoder = PacketDecoder(packet)
        # When
        p = decoder.decode_packet()
        # Then
        self.assertEqual("OutChannel", p.get_receiver())
        self.assertEqual("ReverserChannel", p.get_sender())

    def test_when_packet_decode_packet_should_store_events_correctly(self):
        # Given
        packet = b'DGRPOutChannel\x00SENDReverserChannel\x00LOGAFill Time' \
                 b'\x00\x00\x00\x00\n\x00\x00\x08\xe3?\xf0\x00\x00\x00\x00' \
                 b'\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xe6ffffff' \
                 b'LOGAPacket Pushed\x00\x00\x00\x00\n\x00\x00\t8?\xf0' \
                 b'\x00\x00\x00\x00\x00\x00@\x8c \x00' \
                 b'\x00\x00\x00\x00@\x88\x0733333'

        decoder = PacketDecoder(packet)
        # When
        p = decoder.decode_packet()
        # Then
        event1 = p.get_events()[0]
        event2 = p.get_events()[1]
        self.__check_event(event1, "Fill Time", 10, 2275, 1.0, 1.0, 0.7)
        self.__check_event(event2, "Packet Pushed", 10, 2360, 1.0, 900.0, 768.9)

    def __check_event(self,event,  event_type, log_cnt, p_time, min_val, max_val, avg_val):
        self.assertEqual(event.get_type(), event_type)
        self.assertEqual(event.get_log_count(), log_cnt)
        self.assertEqual(event.get_passed_time(), p_time)
        self.assertEqual(event.get_min(), min_val)
        self.assertEqual(event.get_max(), max_val)
        self.assertEqual(event.get_avg(), avg_val)