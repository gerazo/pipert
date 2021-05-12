import unittest
from src.packet import Packet
from src.event import Event


class TestPacket(unittest.TestCase):
    def test_when_init_should_store_fields(self):
        # Given
        p = Packet("receiver", "    sender", [])
        # When
        rec = p.get_receiver()
        sen = p.get_sender()
        events = p.get_events()
        # Then
        self.assertEqual(rec, "receiver")
        self.assertEqual(sen, "sender")
        self.assertEqual(events, [])

    def test_when_add_events_should_store_new_events(self):
        # Given
        p = Packet("receiver", "    sender", [1])
        # When
        p.add_events([2, 3])
        # Then
        self.assertEqual(p.get_events(), [1, 2, 3])

    def test_when_set_id_should_store_id(self):
        # Given
        p = Packet("receiver", "    sender", [])
        # When
        p.set_id(11)
        # Then
        self.assertEqual(p.get_id(), 11)

    def test_when_get_as_dict_should_return_packet_dict(self):
        # Given
        e1 = Event("LOGAA", 1, 1, 1, 1, 1)
        p = Packet("receiver", "    sender", [e1])
        # When
        packet_dict = p.get_dict()
        # Then
        e1_dict = {
             "type": "A",
             "log_count": 1,
             "passed_time": 1,
             "max": 1,
             "min": 1,
             "avg": 1,
        }
        expected_dict = {
            "receiver_channel": "receiver",
            "sender_channel": "sender",
            "events": [e1_dict],
        }
        self.assertEqual(packet_dict, expected_dict)

    def test_when_set_id_for_events_should_stores_in_events(self):
        # Given
        e1 = Event("LOGAA", 1, 1, 1, 1, 1)
        e2 = Event("LOGAB", 1, 1, 1, 1, 1)
        p = Packet("receiver", "    sender", [e1, e2])
        # When
        p.set_id(11)
        p.set_id_for_events()
        # Then
        self.assertEqual(e1.get_packet_id(), 11)
        self.assertEqual(e2.get_packet_id(), 11)

    def test_when_get_existed_event_count_should_return_log_count(self):
        # Given
        e1 = Event("LOGAA", 2, 1, 1, 1, 1)
        e2 = Event("LOGAB", 3, 1, 1, 1, 1)
        p = Packet("receiver", "    sender", [e1, e2])
        # When
        event_a_log_count = p.get_event_count("A")
        event_b_log_count = p.get_event_count("B")
        # Then
        self.assertEqual(event_a_log_count, 2)
        self.assertEqual(event_b_log_count, 3)

    def test_when_get_no_existed_event_count_should_return_zero(self):
        # Given
        p = Packet("receiver", "    sender", [])
        # When
        event_a_log_count = p.get_event_count("A")
        event_b_log_count = p.get_event_count("B")
        # Then
        self.assertEqual(event_a_log_count, 0)
        self.assertEqual(event_b_log_count, 0)
